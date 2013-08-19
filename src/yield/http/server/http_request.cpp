// yield/http/server/http_request.cpp

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

#include "yield/http/http_response.hpp"
#include "yield/http/server/http_connection.hpp"
#include "yield/http/server/http_request.hpp"
#include "yield/sockets/socket_address.hpp"

namespace yield {
namespace http {
namespace server {
HttpRequest::HttpRequest(
  HttpConnection& connection,
  Method method,
  const yield::uri::Uri& uri,
  YO_NEW_REF Object* body,
  uint8_t http_version
) : yield::http::HttpRequest(method, uri, body, http_version),
    connection(connection.inc_ref()),
    creation_date_time(DateTime::now()) {
}

HttpRequest::HttpRequest(
  YO_NEW_REF Object* body,
  HttpConnection& connection,
  uint16_t fields_offset,
  Buffer& header,
  uint8_t http_version,
  Method method,
  const yield::uri::Uri& uri
)
  : yield::http::HttpRequest(
    body,
    fields_offset,
    header,
    http_version,
    method,
    uri
  ),
  connection(connection.inc_ref()),
  creation_date_time(DateTime::now()) {
}

HttpRequest::~HttpRequest() {
  HttpConnection::dec_ref(connection);
}

void
HttpRequest::respond(
  YO_NEW_REF ::yield::http::HttpResponse& http_response
) {
  connection.handle(http_response);
}

void
HttpRequest::respond(
  ::yield::http::HttpMessageBodyChunk& http_message_body_chunk
) {
  connection.handle(http_message_body_chunk);
}

void HttpRequest::respond(uint16_t status_code) {
  respond(status_code, static_cast<Buffer*>(NULL));
}

void HttpRequest::respond(uint16_t status_code, YO_NEW_REF Buffer* body) {
  HttpResponse* http_response = new HttpResponse(status_code, body, get_http_version());
  if (body != NULL) {
    http_response->set_field("Content-Length", 14, body->size());
  } else {
    http_response->set_field("Content-Length", 14, "0", 1);
  }
  respond(*http_response);
}

void HttpRequest::respond(uint16_t status_code, YO_NEW_REF Buffer& body) {
  HttpResponse* http_response = new HttpResponse(status_code, &body, get_http_version());
  http_response->set_field("Content-Length", 14, body.size());
  respond(*http_response);
}

void HttpRequest::respond(uint16_t status_code, const char* body) {
  respond(status_code, Buffer::copy(body));
}

void HttpRequest::respond(uint16_t status_code, const Exception& body) {
  respond(status_code, Buffer::copy(body.get_error_message()));
}

void HttpRequest::respond(uint16_t status_code, YO_NEW_REF Object* body) {
  respond(*new HttpResponse(status_code, body, get_http_version()));
}

void HttpRequest::respond(uint16_t status_code, YO_NEW_REF Object& body) {
  respond(status_code, &body);
}
}
}
}
