// yield/http/server/http_connection.cpp

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

#include "./http_server_request_parser.hpp"
#include "yield/logging.hpp"
#include "yield/fs/file.hpp"
#include "yield/http/server/http_server_connection.hpp"

namespace yield {
namespace http {
namespace server {
using ::std::make_shared;
using ::std::shared_ptr;
using ::std::unique_ptr;
using ::yield::fs::File;
using ::yield::sockets::SocketAddress;
using ::yield::sockets::TcpSocket;
using ::yield::sockets::aio::AcceptAiocb;
using ::yield::sockets::aio::RecvAiocb;
using ::yield::sockets::aio::SendAiocb;
using ::yield::sockets::aio::SendfileAiocb;

void HttpServerConnection::handle(unique_ptr<AcceptAiocb> accept_aiocb) {
  if (
    accept_aiocb->recv_buffer() != NULL
    &&
    accept_aiocb->return_() > 0
  ) {
    parse(*accept_aiocb->recv_buffer());
  } else {
    shared_ptr<Buffer> recv_buffer
    = make_shared<Buffer>(Buffer::getpagesize(), Buffer::getpagesize());
    unique_ptr<RecvAiocb> recv_aiocb(new RecvAiocb(socket_, *recv_buffer, 0, this));
    if (!aio_queue_->enqueue(move(recv_aiocb))) {
      state_ = STATE_ERROR;
    }
  }
}

void
HttpServerConnection::handle(
  unique_ptr<::yield::http::HttpMessageBodyChunk> http_message_body_chunk
) {
  Buffer* send_buffer;
  if (http_message_body_chunk.data() != NULL) {
    send_buffer = &http_message_body_chunk.data()->inc_ref();
  } else {
    send_buffer = &Buffer::copy("0\r\n\r\n", 5);
  }
  HttpMessageBodyChunk::dec_ref(http_message_body_chunk);

  SendAiocb* send_aiocb = new SendAiocb(socket_, *send_buffer, 0, this);
  if (!aio_queue.enqueue(*send_aiocb)) {
    SendAiocb::dec_ref(*send_aiocb);
    state = STATE_ERROR;
  }
}

void
HttpServerConnection::handle(
  unique_ptr<HttpServerResponse> http_response
) {
  DLOG(DEBUG) << "sending " << http_response;

  http_response.finalize();
  Buffer& http_response_header = http_response.header().inc_ref();
  Buffer* http_response_body_buffer = Object::inc_ref(http_response.body_buffer());
  File* http_response_body_file = Object::inc_ref(http_response.body_file());
  HttpResponse::dec_ref(http_response);

  if (http_response_body_buffer != NULL) {
    CHECK_EQ(http_response_body_file, NULL);
    http_response_header.set_next_buffer(
      static_cast<Buffer*>(http_response_body_buffer)
      );
  } else if (http_response_body_file != NULL) {
    SendAiocb* send_aiocb
      = new SendAiocb(socket_, http_response_header, 0, this);
    if (aio_queue.enqueue(*send_aiocb)) {
      SendfileAiocb* sendfile_aiocb
      = new SendfileAiocb(socket_, *static_cast<File*>(http_response_body_file), this);
      if (aio_queue.enqueue(*sendfile_aiocb)) {
        return;
      } else {
        SendfileAiocb::dec_ref(*sendfile_aiocb);
        state = STATE_ERROR;
      }
    } else {
      SendAiocb::dec_ref(*send_aiocb);
      state = STATE_ERROR;
    }
  }

  SendAiocb* send_aiocb = new SendAiocb(socket_, http_response_header, 0, this);
  if (!aio_queue.enqueue(*send_aiocb)) {
    SendAiocb::dec_ref(*send_aiocb);
    state = STATE_ERROR;
  }
}

void
HttpServerConnection::handle(
  unique_ptr< ::yield::sockets::aio::RecvAiocb > recv_aiocb
) {
  if (recv_aiocb->return_() > 0) {
    parse(*recv_aiocb->buffer());
  }
}

void
HttpServerConnection::handle(
  unique_ptr< ::yield::sockets::aio::SendAiocb > send_aiocb
) {
}

void
HttpServerConnection::handle(
  unique_ptr< ::yield::sockets::aio::SendfileAiocb > sendfile_aiocb
) {
}

void HttpServerConnection::parse(Buffer& recv_buffer) {
  CHECK(!recv_buffer.empty());

  HttpRequestParser http_request_parser(*this, recv_buffer);

  for (;;) {
    Object& object = http_request_parser.parse();

    switch (object.get_type_id()) {
    case Buffer::TYPE_ID: {
      Buffer& next_recv_buffer = static_cast<Buffer&>(object);
      RecvAiocb* recv_aiocb = new RecvAiocb(socket_, next_recv_buffer, 0, this);
      if (!aio_queue.enqueue(*recv_aiocb)) {
        RecvAiocb::dec_ref(*recv_aiocb);
        state = STATE_ERROR;
      }
    }
    return;

    case HttpRequest::TYPE_ID: {
      HttpRequest& http_request = static_cast<HttpRequest&>(object);

      DLOG(DEBUG) << "parsed " << http_request;

      http_request_handler.handle(http_request);
    }
    break;

    case HttpResponse::TYPE_ID: {
      HttpResponse& http_response = static_cast<HttpResponse&>(object);
      CHECK_EQ(http_response.status_code(), 400);

      DLOG(DEBUG) << "parsed " << http_response;

      handle(http_response);
      return;
    }
    break;

    default:
      CHECK(false);
      break;
    }
  }
}
}
}
}
