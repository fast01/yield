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

#ifndef _YIELD_HTTP_SERVER_HTTP_SERVER_REQUEST_HPP_
#define _YIELD_HTTP_SERVER_HTTP_SERVER_REQUEST_HPP_

#include <memory>

#include "yield/http/http_request.hpp"
#include "yield/http/server/http_server_event.hpp"
#include "yield/http/server/http_server_connection.hpp"
#include "yield/http/server/http_server_response.hpp"

namespace yield {
namespace http {
class HttpMessageBodyChunk;
class HttpResponse;

namespace server {
class HttpServerRequestParser;

/**
  A <code>yield::http::HttpRequest</code> that's bound to a server
    <code>HttpServerConnection</code>.
  These <code>HttpRequest</code>s are usually created by
    <code>HttpServerRequestQueue</code>.
*/
class HttpServerRequest final : public ::yield::http::HttpRequest, public HttpServerEvent {
public:
  /**
    Construct an HttpRequest that originates from the given server connection.
    @param connection the server connection
    @param method the HTTP request method e.g., HttpRequest::Method::GET
    @param uri the HTTP request Uri e.g., /
    @param body an optional body, usually a Buffer
    @param http_version the HTTP version as a single byte (0 or 1 for HTTP/1.0
      and HTTP/1.1, respectively)
  */
  HttpServerRequest(
    ::std::shared_ptr<HttpServerConnection> connection,
    Method method,
    const yield::uri::Uri& uri,
    ::std::shared_ptr<Buffer> body = NULL,
    uint8_t http_version = HTTP_VERSION_DEFAULT
  ) : yield::http::HttpRequest(method, uri, body, http_version),
    connection_(connection),
    creation_date_time_(DateTime::now()) {
  }

public:
  /**
    Get the server connection from which this HTTP request originated.
    @return the server connection from which this HTTP request originated
  */
  const HttpServerConnection& connection() const {
    return *connection_;
  }

  /**
    Get the date-time this HttpRequest was constructed.
    Used in logging.
    @return the date-time this HttpRequest was constructed
  */
  const DateTime& creation_date_time() const {
    return creation_date_time_;
  }

public:
  /**
    Respond to the HTTP request with a chunked body.
    respond(HttpResponse&) or respond(status_code) must be called first.
    @param chunk response body chunk
  */
  void respond(::std::unique_ptr<HttpServerResponse> response) {
    connection_->handle(::std::move(response));
  }

  /**
    Respond to the HTTP request with a chunked body.
    respond(HttpResponse&) or respond(status_code) must be called first.
    @param chunk response body chunk
  */
  void respond(::std::unique_ptr<HttpServerMessageBodyChunk> chunk) {
    connection_->handle(::std::move(chunk));
  }

  /**
    Respond to the HTTP request.
    @param status_code response status code e.g, 200
  */
  void respond(uint16_t status_code) {
    respond(status_code, ::std::shared_ptr<Buffer>());
  }

  /**
    Respond to the HTTP request.
    This method should only be called once.
    @param status_code response status code e.g., 200
    @param body response body
  */
  void respond(uint16_t status_code, ::std::shared_ptr<Buffer> body);

  /**
    Respond to the HTTP request.
    This method should only be called once.
    @param status_code response status code e.g., 200
    @param body response body
  */
  void respond(uint16_t status_code, const char* body) {
    respond(status_code, Buffer::copy(body));
  }

  /**
    Respond to the HTTP request.
    This method should only be called once.
    @param status_code response status code e.g., 200
    @param body response body
  */
  void respond(uint16_t status_code, const Exception& body) {
    respond(status_code, Buffer::copy(body.get_error_message()));
  }

public:
  Type type() const override {
    return Type::REQUEST;
  }

protected:
  friend class HttpServerRequestParser;

  HttpServerRequest(
    ::std::shared_ptr<Buffer> body,
    ::std::shared_ptr<HttpServerConnection> connection,
    uint16_t fields_offset,
    ::std::shared_ptr<Buffer> header,
    uint8_t http_version,
    Method method,
    const yield::uri::Uri& uri
  ) : yield::http::HttpRequest(
    body,
    fields_offset,
    header,
    http_version,
    method,
    uri
  ),
  connection_(connection),
  creation_date_time_(DateTime::now()) {
  }

private:
  ::std::shared_ptr<HttpServerConnection> connection_;
  DateTime creation_date_time_;
};
}
}
}

#endif
