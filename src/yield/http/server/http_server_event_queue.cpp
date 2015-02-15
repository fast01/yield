// Copyright (c) 2014 Minor Gordon
// All rights reserved

// This source file is part of the Yield project.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// * Neither the name of the Yield project nor the
// names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL Minor Gordon BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "yield/http/server/http_server_event_queue.hpp"

#include "yield/logging.hpp"
#include "yield/http/server/http_server_connection.hpp"
#include "yield/sockets/tcp_socket.hpp"
#include "yield/sockets/aio/accept_aiocb.hpp"
#include "yield/sockets/aio/socket_aio_queue.hpp"

namespace yield {
namespace http {
namespace server {
using ::std::make_shared;
using ::std::move;
using ::std::shared_ptr;
using ::std::unique_ptr;
using ::std::vector;
using ::yield::sockets::Socket;
using ::yield::sockets::SocketAddress;
using ::yield::sockets::StreamSocket;
using ::yield::sockets::TcpSocket;
using ::yield::sockets::aio::AcceptAiocb;
using ::yield::sockets::aio::SocketAioQueue;
using ::yield::sockets::aio::RecvAiocb;
using ::yield::sockets::aio::SocketAiocb;

HttpServerEventQueue::HttpServerEventQueue(
  const SocketAddress& sockname
) throw(Exception) : aio_queue_(SocketAioQueue::create()),
  socket_(make_shared<TcpSocket>(sockname.get_family())) {
  init(sockname);
}

HttpServerEventQueue::HttpServerEventQueue(
  ::std::unique_ptr<TcpSocket> socket_,
  const SocketAddress& sockname
) throw(Exception) : aio_queue_(SocketAioQueue::create()),
  socket_(socket_.release()) {
  init(sockname);
}

HttpServerEventQueue::~HttpServerEventQueue() {
  for (auto connection_i = connections_.begin(); connection_i != connections_.end(); ++connection_i) {
    StreamSocket& socket_ = (*connection_i)->socket();
    socket_.set_blocking_mode(true);
    socket_.setsockopt(TcpSocket::Option::LINGER, 30);
    socket_.shutdown();
    socket_.close();
  }

  socket_->close();
}

void HttpServerEventQueue::handle(unique_ptr<AcceptAiocb> accept_aiocb) {
  if (accept_aiocb->return_() >= 0) {
    if (aio_queue_->associate(*accept_aiocb->accepted_socket())) {
      HttpServerConnection* connection
      = new HttpServerConnection(
        aio_queue_,
        event_queue_,
        accept_aiocb->peername(),
        accept_aiocb->accepted_socket()
      );

      connection->handle(move(accept_aiocb));

      if (connection->state() == HttpServerConnection::STATE_CONNECTED) {
        connections_.push_back(connection->shared_from_this());
      } else {
        delete connection;
      }
    } else {
      accept_aiocb->accepted_socket()->shutdown();
      accept_aiocb->accepted_socket()->close();
    }
  }

  unique_ptr<AcceptAiocb> next_accept_aiocb(new AcceptAiocb(socket_, make_shared<Buffer>(Buffer::getpagesize(), Buffer::getpagesize())));
  aio_queue_->tryenqueue(move(next_accept_aiocb));
}


template <class AiocbType>
void HttpServerEventQueue::handle(unique_ptr<AiocbType> aiocb) {
  

  HttpServerConnection& connection = *static_cast<HttpServerConnection*>(aiocb.get_context());
  if (connection.get_state() == HttpServerConnection::STATE_CONNECTED) {
    connection.handle(aiocb);

    if (connection.get_state() == HttpServerConnection::STATE_ERROR) {
      for (
        vector<HttpServerConnection*>::iterator connection_i = connections.begin();
        connection_i != connections.end();
        ++connection_i
      ) {
        if (*connection_i == &connection) {
          connections.erase(connection_i);
          connection.get_socket().close();
          HttpServerConnection::dec_ref(connection);
        }
      }
    }
  } else {
    AiocbType::dec_ref(aiocb);
  }
}


void
HttpServerEventQueue::init(
  const SocketAddress& sockname
) throw(Exception) {
  event_queue_ = make_shared< ::yield::stage::SynchronizedEventQueue<HttpServerEvent> >();

  if (!aio_queue_->associate(*socket_)) {
    throw Exception();
  }
#ifdef __linux__
  if (!socket_->setsockopt(Socket::Option::REUSEADDR, true)) {
    throw Exception();
  }
#endif
  if (!socket_->bind(sockname)) {
    throw Exception();
  }
  if (!socket_->listen()) {
    throw Exception();
  }

  unique_ptr<AcceptAiocb> accept_aiocb(new AcceptAiocb(socket_, make_shared<Buffer>(Buffer::getpagesize(), Buffer::getpagesize())));
  aio_queue_->tryenqueue(move(accept_aiocb));
}


unique_ptr<HttpServerEvent> HttpServerEventQueue::timeddequeue(const Time& timeout) {
  Time timeout_remaining(timeout);
  for (;;) {
    Time start_time = Time::now();

    unique_ptr<SocketAiocb> aiocb = aio_queue_->timeddequeue(timeout_remaining);
    if (aiocb != NULL) {
      switch (aiocb->type()) {
      case SocketAiocb::Type::ACCEPT:
        handle(unique_ptr<AcceptAiocb>(static_cast<AcceptAiocb*>(aiocb.release())));
        break;

      case SocketAiocb::Type::RECV:
        handle(unique_ptr<RecvAiocb>(static_cast<RecvAiocb*>(aiocb.release())));
        break;

      case SocketAiocb::Type::SEND:
        break;

      case SocketAiocb::Type::SENDFILE:
        break;

      default:
        CHECK(false);
        break;
      }
    } else if (timeout_remaining > static_cast<uint64_t>(0)) {
      Time elapsed_time = Time::now() - start_time;
      if (timeout_remaining > elapsed_time) {
        timeout_remaining -= elapsed_time;
      } else {
        timeout_remaining = 0;
      }
    } else {
      return NULL;
    }
  }
}

unique_ptr<HttpServerEvent> HttpServerEventQueue::tryenqueue(unique_ptr<HttpServerEvent> event) {
  return event_queue_->tryenqueue(move(event));
}
}
}
}
