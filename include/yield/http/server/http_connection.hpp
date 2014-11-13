// yield/http/server/http_connection.hpp

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

#ifndef _YIELD_HTTP_SERVER_HTTP_CONNECTION_HPP_
#define _YIELD_HTTP_SERVER_HTTP_CONNECTION_HPP_

#include "yield/event_queue.hpp"
#include "yield/sockets/aio/accept_aiocb.hpp"
#include "yield/sockets/aio/recv_aiocb.hpp"
#include "yield/sockets/aio/send_aiocb.hpp"
#include "yield/sockets/aio/sendfile_aiocb.hpp"
#include "yield/sockets/tcp_socket.hpp"
#include "yield/http/server/http_message_body_chunk.hpp"
#include "yield/http/server/http_request.hpp"
#include "yield/http/server/http_response.hpp"

namespace yield {
namespace http {
namespace server {
/**
  A server-side HTTP connection_.
*/
class HttpConnection : public Object {
public:
  enum State { STATE_CONNECTED, STATE_ERROR };

public:
  HttpConnection(
    EventQueue& aio_queue,
    EventHandler& http_request_handler,
    yield::sockets::SocketAddress& peername,
    yield::sockets::TcpSocket& socket_
  );

  ~HttpConnection();

public:
  EventHandler& get_http_request_handler() {
    return http_request_handler;
  }

  yield::sockets::SocketAddress& get_peername() const {
    return peername;
  }

  yield::sockets::TcpSocket& get_socket() const {
    return socket_;
  }

  State get_state() const {
    return state;
  }

public:
  void handle(YO_NEW_REF ::yield::sockets::aio::AcceptAiocb& accept_aiocb);
  void handle(YO_NEW_REF ::yield::http::HttpMessageBodyChunk& http_message_body_chunk);
  void handle(YO_NEW_REF ::yield::http::HttpResponse& http_response);
  void handle(YO_NEW_REF ::yield::sockets::aio::RecvAiocb& recv_aiocb);
  void handle(YO_NEW_REF ::yield::sockets::aio::SendAiocb& send_aiocb);
  void handle(YO_NEW_REF ::yield::sockets::aio::SendfileAiocb& sendfile_aiocb);

public:
  // yield::Object
  HttpConnection& inc_ref() {
    return Object::inc_ref(*this);
  }

private:
  void parse(Buffer& recv_buffer);

private:
  EventQueue& aio_queue;
  EventHandler& http_request_handler;
  yield::sockets::SocketAddress& peername;
  yield::sockets::TcpSocket& socket_;
  State state;
};
}
}
}

#endif
