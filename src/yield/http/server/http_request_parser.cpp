// yield/http/server/http_request_parser.cpp

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

#include "http_request_parser.hpp"
#include "yield/http/server/http_connection.hpp"
#include "yield/http/server/http_request.hpp"
#include "yield/http/server/http_message_body_chunk.hpp"
#include "yield/sockets/socket_address.hpp"

namespace yield {
namespace http {
namespace server {
using yield::sockets::SocketAddress;

HttpRequestParser::HttpRequestParser(HttpConnection& connection, Buffer& data)
  : yield::http::HttpRequestParser(data),
    connection_(connection.inc_ref()) {
}

HttpRequestParser::~HttpRequestParser() {
  HttpConnection::dec_ref(connection_);
}

YO_NEW_REF yield::http::HttpMessageBodyChunk&
HttpRequestParser::create_http_message_body_chunk(
  YO_NEW_REF Buffer* data
) {
  return *new HttpMessageBodyChunk(connection_, data);
}

YO_NEW_REF yield::http::HttpRequest&
HttpRequestParser::create_http_request(
  YO_NEW_REF Buffer* body,
  uint16_t fields_offset,
  Buffer& header,
  uint8_t http_version,
  HttpRequest::Method method,
  const yield::uri::Uri& uri
) {
  return *new HttpRequest(
           body,
           connection_,
           fields_offset,
           header,
           http_version,
           method,
           uri
         );
}
}
}
}
