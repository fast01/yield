// yield/http/server/http_request_queue.cpp

// Copyright (c) 2013 Minor Gordon
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

#include "yield/logging.hpp"
#include "yield/http/server/http_connection.hpp"
#include "yield/http/server/http_request_queue.hpp"
#include "yield/sockets/tcp_socket.hpp"
#include "yield/sockets/aio/accept_aiocb.hpp"
#include "yield/sockets/aio/aio_queue.hpp"
#include "yield/sockets/aio/nbio_queue.hpp"

namespace yield {
namespace http {
namespace server {
using ::std::vector;
using ::yield::sockets::Socket;
using ::yield::sockets::SocketAddress;
using ::yield::sockets::TcpSocket;
using ::yield::sockets::aio::AcceptAiocb;
using ::yield::sockets::aio::AioQueue;
using ::yield::sockets::aio::RecvAiocb;
using ::yield::sockets::aio::SendAiocb;
using ::yield::sockets::aio::SendfileAiocb;

template <class AioQueueType>
HttpRequestQueue<AioQueueType>::HttpRequestQueue(
  const SocketAddress& sockname
) throw(Exception) : aio_queue(*new AioQueueType()),
  socket_(*new TcpSocket(sockname.get_family())) {
  init(sockname);
}

template <class AioQueueType>
HttpRequestQueue<AioQueueType>::HttpRequestQueue(
  YO_NEW_REF TcpSocket& socket_,
  const SocketAddress& sockname
) throw(Exception) : aio_queue(*new AioQueueType()),
  socket_(socket_) {
  init(sockname);
}

template <class AioQueueType>
HttpRequestQueue<AioQueueType>::~HttpRequestQueue() {
  for (
    vector<HttpConnection*>::iterator connection_i = connections.begin();
    connection_i != connections.end();
    ++connection_i
  ) {
    HttpConnection* connection = *connection_i;
    TcpSocket& socket_ = connection->get_socket();
    socket_.set_blocking_mode(true);
    socket_.setsockopt(TcpSocket::Option::LINGER, 30);
    socket_.shutdown();
    socket_.close();
    HttpConnection::dec_ref(*connection);
  }

  socket_.close();

  AioQueue::dec_ref(aio_queue);
  TcpSocket::dec_ref(socket_);
}

template <class AioQueueType>
bool HttpRequestQueue<AioQueueType>::enqueue(YO_NEW_REF Event& event) {
  return aio_queue.enqueue(event);
}

template <class AioQueueType>
void HttpRequestQueue<AioQueueType>::handle(YO_NEW_REF AcceptAiocb& accept_aiocb) {
  if (accept_aiocb.get_return() >= 0) {
    TcpSocket& accepted_socket
    = static_cast<TcpSocket&>(*accept_aiocb.get_accepted_socket());

    if (aio_queue.associate(accepted_socket)) {
      HttpConnection* connection
      = new HttpConnection(
        aio_queue,
        aio_queue,
        *accept_aiocb.get_peername(),
        static_cast<TcpSocket&>(accepted_socket)
      );

      connection->handle(accept_aiocb);

      if (connection->get_state() == HttpConnection::STATE_CONNECTED) {
        connections.push_back(connection);
      } else {
        HttpConnection::dec_ref(*connection);
      }
    } else {
      accepted_socket.shutdown();
      accepted_socket.close();
      TcpSocket::dec_ref(accepted_socket);
    }
  } else {
    AcceptAiocb::dec_ref(accept_aiocb);
  }

  Buffer* recv_buffer
  = new Buffer(Buffer::getpagesize(), Buffer::getpagesize());
  AcceptAiocb* next_accept_aiocb = new AcceptAiocb(socket_, recv_buffer);
  if (!aio_queue.enqueue(*next_accept_aiocb)) {
    AcceptAiocb::dec_ref(next_accept_aiocb);
  }
}

template <class AioQueueType>
template <class AiocbType>
void HttpRequestQueue<AioQueueType>::handle(YO_NEW_REF AiocbType& aiocb) {
  HttpConnection& connection = *static_cast<HttpConnection*>(aiocb.get_context());
  if (connection.get_state() == HttpConnection::STATE_CONNECTED) {
    connection.handle(aiocb);

    if (connection.get_state() == HttpConnection::STATE_ERROR) {
      for (
        vector<HttpConnection*>::iterator connection_i = connections.begin();
        connection_i != connections.end();
        ++connection_i
      ) {
        if (*connection_i == &connection) {
          connections.erase(connection_i);
          connection.get_socket().close();
          HttpConnection::dec_ref(connection);
        }
      }
    }
  } else {
    AiocbType::dec_ref(aiocb);
  }
}

template <class AioQueueType>
void
HttpRequestQueue<AioQueueType>::init(
  const SocketAddress& sockname
) throw(Exception) {
  if (aio_queue.associate(socket_)) {
#ifdef __linux__
    if (socket_.setsockopt(Socket::Option::REUSEADDR, true)) {
#endif
      if (socket_.bind(sockname)) {
        if (socket_.listen()) {
          Buffer* recv_buffer
          = new Buffer(Buffer::getpagesize(), Buffer::getpagesize());
          AcceptAiocb* accept_aiocb = new AcceptAiocb(socket_, recv_buffer);
          if (aio_queue.enqueue(*accept_aiocb)) {
            return;
          }
        }
      }
#ifdef __linux__
    }
#endif
  }

  throw Exception();
}

template <class AioQueueType>
YO_NEW_REF Event* HttpRequestQueue<AioQueueType>::timeddequeue(const Time& timeout) {
  Time timeout_remaining(timeout);

  for (;;) {
    Time start_time = Time::now();

    Event* event = aio_queue.timeddequeue(timeout_remaining);

    if (event != NULL) {
      switch (event->get_type_id()) {
      case AcceptAiocb::TYPE_ID: {
        handle(static_cast<AcceptAiocb&>(*event));
      }
      break;

      case RecvAiocb::TYPE_ID: {
        handle<RecvAiocb>(static_cast<RecvAiocb&>(*event));
      }
      break;

      case SendAiocb::TYPE_ID: {
        handle<SendAiocb>(static_cast<SendAiocb&>(*event));
      }
      break;

      case SendfileAiocb::TYPE_ID: {
        handle<SendfileAiocb>(static_cast<SendfileAiocb&>(*event));
      }
      break;

      default:
        return event;
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

#ifdef _WIN32
template class HttpRequestQueue<yield::sockets::aio::AioQueue>;
#endif
template class HttpRequestQueue<yield::sockets::aio::NbioQueue>;
}
}
}
