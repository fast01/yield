// yield/http/server/http_connection.cpp

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

#include "http_request_parser.hpp"
#include "yield/debug.hpp"
#include "yield/log.hpp"
#include "yield/fs/file.hpp"
#include "yield/http/server/http_connection.hpp"

namespace yield {
namespace http {
namespace server {
using yield::fs::File;
using yield::sockets::SocketAddress;
using yield::sockets::TcpSocket;
using yield::sockets::aio::AcceptAiocb;
using yield::sockets::aio::RecvAiocb;
using yield::sockets::aio::SendAiocb;
using yield::sockets::aio::SendfileAiocb;

HttpConnection::HttpConnection(
  EventQueue& aio_queue,
  EventHandler& http_request_handler,
  SocketAddress& peername,
  TcpSocket& socket_,
  Log* log
) : aio_queue(aio_queue.inc_ref()),
  http_request_handler(http_request_handler.inc_ref()),
  log(Object::inc_ref(log)),
  peername(peername.inc_ref()),
  socket_(static_cast<TcpSocket&>(socket_.inc_ref())) {
  state = STATE_CONNECTED;
}

HttpConnection::~HttpConnection() {
  EventQueue::dec_ref(aio_queue);
  EventHandler::dec_ref(http_request_handler);
  Log::dec_ref(log);
  TcpSocket::dec_ref(socket_);
}

void HttpConnection::handle(YO_NEW_REF AcceptAiocb& accept_aiocb) {
  if (
    accept_aiocb.get_recv_buffer() != NULL
    &&
    accept_aiocb.get_return() > 0
  ) {
    parse(*accept_aiocb.get_recv_buffer());
  } else {
    Buffer* recv_buffer
    = new Buffer(Buffer::getpagesize(), Buffer::getpagesize());
    RecvAiocb* recv_aiocb = new RecvAiocb(socket_, *recv_buffer, 0, this);
    if (!aio_queue.enqueue(*recv_aiocb)) {
      RecvAiocb::dec_ref(*recv_aiocb);
      state = STATE_ERROR;
    }
  }

  AcceptAiocb::dec_ref(accept_aiocb);
}

void
HttpConnection::handle(
  YO_NEW_REF ::yield::http::HttpMessageBodyChunk& http_message_body_chunk
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
HttpConnection::handle(
  YO_NEW_REF ::yield::http::HttpResponse& http_response
) {
  if (log != NULL) {
    log->get_stream(Log::Level::DEBUG) << get_type_name()
                                       << ": sending " << http_response;
  }

  http_response.finalize();
  Buffer& http_response_header = http_response.get_header().inc_ref();
  Object* http_response_body = Object::inc_ref(http_response.get_body());
  HttpResponse::dec_ref(http_response);

  if (http_response_body != NULL) {
    switch (http_response_body->get_type_id()) {
    case Buffer::TYPE_ID: {
      http_response_header.set_next_buffer(
        static_cast<Buffer*>(http_response_body)
      );
    }
    break;

    case File::TYPE_ID: {
      SendAiocb* send_aiocb
        = new SendAiocb(socket_, http_response_header, 0, this);
      if (aio_queue.enqueue(*send_aiocb)) {
        SendfileAiocb* sendfile_aiocb
        = new SendfileAiocb(socket_, *static_cast<File*>(http_response_body), this);
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
    break;

    default:
      debug_break();
    }
  }

  SendAiocb* send_aiocb = new SendAiocb(socket_, http_response_header, 0, this);
  if (!aio_queue.enqueue(*send_aiocb)) {
    SendAiocb::dec_ref(*send_aiocb);
    state = STATE_ERROR;
  }
}

void
HttpConnection::handle(
  YO_NEW_REF ::yield::sockets::aio::RecvAiocb& recv_aiocb
) {
  if (recv_aiocb.get_return() > 0) {
    parse(recv_aiocb.get_buffer());
  }

  ::yield::sockets::aio::RecvAiocb::dec_ref(recv_aiocb);
}

void
HttpConnection::handle(
  YO_NEW_REF ::yield::sockets::aio::SendAiocb& send_aiocb
) {
  SendAiocb::dec_ref(send_aiocb);
}

void
HttpConnection::handle(
  YO_NEW_REF ::yield::sockets::aio::SendfileAiocb& sendfile_aiocb
) {
  ::yield::sockets::aio::SendfileAiocb::dec_ref(sendfile_aiocb);
}

void HttpConnection::parse(Buffer& recv_buffer) {
  debug_assert_false(recv_buffer.empty());

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

      if (log != NULL) {
        log->get_stream(Log::Level::DEBUG) << get_type_name()
                                           << ": parsed " << http_request;
      }

      http_request_handler.handle(http_request);
    }
    break;

    case HttpResponse::TYPE_ID: {
      HttpResponse& http_response = static_cast<HttpResponse&>(object);
      debug_assert_eq(http_response.get_status_code(), 400);

      if (log != NULL) {
        log->get_stream(Log::Level::DEBUG) << get_type_name()
                                           << ": parsed " << http_response;
      }

      handle(http_response);
      return;
    }
    break;

    default:
      debug_break();
      break;
    }
  }
}
}
}
}
