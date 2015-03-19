// Copyright (c) 2015 Minor Gordon
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

#ifndef _YIELD_HTTP_SERVER_HTTP_CONNECTION_HPP_
#define _YIELD_HTTP_SERVER_HTTP_CONNECTION_HPP_

#include "yield/event_queue.hpp"
#include "yield/sockets/aio/socket_aio_queue.hpp"
#include "yield/sockets/aio/accept_aiocb.hpp"
#include "yield/sockets/aio/recv_aiocb.hpp"
#include "yield/sockets/tcp_socket.hpp"
#include "yield/http/server/http_server_message_body_chunk.hpp"
#include "yield/http/server/http_server_response.hpp"

namespace yield {
namespace http {
namespace server {
class HttpServerRequest;

/**
  A server-side HTTP connection.
*/
class HttpServerConnection final : private ::std::enable_shared_from_this<HttpServerConnection> {
public:
  enum class State { CONNECTED, ERROR };

public:
  ::yield::sockets::SocketAddress& peername() const {
    return *peername_;
  }

  ::yield::sockets::StreamSocket& socket() const {
    return *socket_;
  }

  State state() const {
    return state_;
  }

public:
  void handle(::std::unique_ptr< ::yield::http::HttpMessageBodyChunk > http_message_body_chunk);
  void handle(::std::unique_ptr<HttpServerResponse> http_response);

private:
  friend class HttpServerEventQueue;

private:
  class ParseCallbacks;

  class RecvAiocb final : public ::yield::sockets::aio::RecvAiocb {
  public:
    RecvAiocb(
      ::std::shared_ptr<Buffer> buffer,
      ::std::shared_ptr<HttpServerConnection> connection,
      const ::yield::sockets::Socket::MessageFlags& flags
    ) : ::yield::sockets::aio::RecvAiocb(connection->socket_, buffer, flags),
        connection_(connection) {
    }

  public:
    ::std::shared_ptr<HttpServerConnection>& connection() {
      return connection_;
    }

  private:
    ::std::shared_ptr<HttpServerConnection> connection_;
  };

private:
  HttpServerConnection(
    ::std::shared_ptr< EventQueue< ::yield::sockets::aio::SocketAiocb > > aio_queue,
    ::std::shared_ptr< EventSink<HttpServerEvent > > event_handler,
    ::std::shared_ptr< ::yield::sockets::SocketAddress > peername,
    ::std::shared_ptr< ::yield::sockets::StreamSocket > socket_
  ) : aio_queue_(aio_queue),
      event_handler_(event_handler),
      peername_(peername),
      socket_(socket_) {
    state_ = State::CONNECTED;
  }

private:
  void handle(::std::unique_ptr< ::yield::sockets::aio::AcceptAiocb > accept_aiocb);
  void handle(::std::unique_ptr< ::yield::sockets::aio::RecvAiocb > recv_aiocb);
  void parse(::std::shared_ptr<Buffer> recv_buffer);

private:
  ::std::shared_ptr< EventQueue< ::yield::sockets::aio::SocketAiocb > >  aio_queue_;
  ::std::shared_ptr< EventSink<HttpServerEvent> > event_handler_;
  ::std::shared_ptr< ::yield::sockets::SocketAddress > peername_;
  ::std::shared_ptr< ::yield::sockets::StreamSocket > socket_;
  State state_;
};
}
}
}

#endif
