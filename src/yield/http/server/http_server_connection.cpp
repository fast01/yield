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

#include "yield/logging.hpp"
#include "yield/fs/file.hpp"
#include "yield/http/http_request_parser.hpp"
#include "yield/http/server/http_server_connection.hpp"
#include "yield/http/server/http_server_request.hpp"
#include "yield/sockets/aio/send_aiocb.hpp"
#include "yield/sockets/aio/sendfile_aiocb.hpp"

namespace yield {
namespace http {
namespace server {
using ::std::make_shared;
using ::std::shared_ptr;
using ::std::unique_ptr;
using ::yield::fs::File;
using ::yield::sockets::Socket;
using ::yield::sockets::SocketAddress;
using ::yield::sockets::StreamSocket;
using ::yield::sockets::TcpSocket;
using ::yield::sockets::aio::AcceptAiocb;
using ::yield::sockets::aio::RecvAiocb;
using ::yield::sockets::aio::SendAiocb;
using ::yield::sockets::aio::SendfileAiocb;

class HttpServerConnection::ParseCallbacks final : public HttpRequestParser::ParseCallbacks {
public:
  ParseCallbacks(HttpServerConnection& connection)
    : connection_(connection) {
  }

  void handle_error_http_response(uint8_t http_version, uint16_t status_code) override {
      unique_ptr<HttpServerResponse> http_response(new HttpServerResponse(http_version, status_code));
      DLOG(DEBUG) << "parsed " << *http_response;
      connection_.handle(move(http_response));
  }

  void handle_http_message_body_chunk(shared_ptr<Buffer> data) override {
  }

  void
  handle_http_request(
    ::std::shared_ptr<Buffer> body,
    uint16_t fields_offset,
    ::std::shared_ptr<Buffer> header,
    uint8_t http_version,
    HttpRequest::Method method,
    const yield::uri::Uri& uri
  ) {
      unique_ptr<HttpServerRequest> http_request(new HttpServerRequest(body, connection_.shared_from_this(), fields_offset, header, http_version, method, uri));
      DLOG(DEBUG) << "parsed " << *http_request;
      connection_.event_handler_->handle(move(http_request));
  }

  void read(shared_ptr<Buffer> buffer) override {
    unique_ptr<RecvAiocb> recv_aiocb(new RecvAiocb(buffer, connection_.shared_from_this(), 0));
    if (connection_.aio_queue_->tryenqueue(move(recv_aiocb))) {
      connection_.state_ = State::ERROR;
    }
  }

private:
  HttpServerConnection& connection_;
};

void HttpServerConnection::handle(unique_ptr<AcceptAiocb> accept_aiocb) {
  if (
    accept_aiocb->recv_buffer() != NULL
    &&
    accept_aiocb->return_() > 0
  ) {
    parse(accept_aiocb->recv_buffer());
  } else {
    shared_ptr<Buffer> recv_buffer
    = make_shared<Buffer>(Buffer::getpagesize(), Buffer::getpagesize());
    unique_ptr<RecvAiocb> recv_aiocb(new RecvAiocb(recv_buffer, shared_from_this(), 0));
    if (aio_queue_->tryenqueue(move(recv_aiocb))) {
      state_ = State::ERROR;
    }
  }
}

void
HttpServerConnection::handle(
  unique_ptr<HttpMessageBodyChunk> http_message_body_chunk
) {
  shared_ptr<Buffer> send_buffer;
  if (http_message_body_chunk->data()) {
    send_buffer = http_message_body_chunk->data();
  } else {
    send_buffer = Buffer::copy("0\r\n\r\n", 5);
  }

  unique_ptr<SendAiocb> send_aiocb(new SendAiocb(shared_ptr<Socket>(socket_), send_buffer, 0));
  if (aio_queue_->tryenqueue(move(send_aiocb))) {
    state_ = State::ERROR;
  }
}

void
HttpServerConnection::handle(
  unique_ptr<HttpServerResponse> http_response
) {
  DLOG(DEBUG) << "sending " << *http_response;

  http_response->finalize();

  if (http_response->body_buffer()) {
    CHECK(!http_response->body_file());
    http_response->header()->set_next_buffer(http_response->body_buffer());
  } else if (http_response->body_file()) {
    unique_ptr<SendAiocb> send_aiocb(new SendAiocb(socket_, http_response->header(), 0));
    if (aio_queue_->tryenqueue(move(send_aiocb)) == NULL) {
      unique_ptr<SendfileAiocb> sendfile_aiocb(new SendfileAiocb(shared_ptr<StreamSocket>(socket_), *http_response->body_file()));
      if (aio_queue_->tryenqueue(move(sendfile_aiocb)) == NULL) {
        return;
      } else {
        state_ = State::ERROR;
      }
    } else {
      state_ = State::ERROR;
    }
  }

  unique_ptr<SendAiocb> send_aiocb(new SendAiocb(socket_, http_response->header(), 0));
  if (aio_queue_->tryenqueue(move(send_aiocb))) {
    state_ = State::ERROR;
  }
}

void
HttpServerConnection::handle(
  unique_ptr< ::yield::sockets::aio::RecvAiocb > recv_aiocb
) {
  if (recv_aiocb->return_() > 0) {
    parse(recv_aiocb->buffer());
  }
}

void HttpServerConnection::parse(shared_ptr<Buffer> recv_buffer) {
  CHECK(!recv_buffer->empty());
  HttpRequestParser parser(recv_buffer);
  ParseCallbacks parse_callbacks(*this);
  parser.parse(parse_callbacks);
}
}
}
}
