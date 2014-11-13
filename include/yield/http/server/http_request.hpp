// yield/http/server/http_request.hpp

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

#ifndef _YIELD_HTTP_SERVER_HTTP_REQUEST_HPP_
#define _YIELD_HTTP_SERVER_HTTP_REQUEST_HPP_

#include "yield/http/http_request.hpp"

namespace yield {
namespace http {
class HttpMessageBodyChunk;
class HttpResponse;

namespace server {
class HttpConnection;
class HttpRequestParser;

/**
  A <code>yield::http::HttpRequest</code> that's bound to a server
    <code>HttpConnection</code>.
  These <code>HttpRequest</code>s are usually created by
    <code>HttpRequestQueue</code>.
*/
class HttpRequest : public ::yield::http::HttpRequest {
public:
  const static uint32_t TYPE_ID = 2792000307UL;

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
  HttpRequest(
    HttpConnection& connection,
    Method method,
    const yield::uri::Uri& uri,
    YO_NEW_REF Buffer* body = NULL,
    uint8_t http_version = HTTP_VERSION_DEFAULT
  );

  virtual ~HttpRequest();

public:
  /**
    Get the server connection from which this HTTP request originated.
    @return the server connection from which this HTTP request originated
  */
  const HttpConnection& connection() const {
    return connection_;
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
  void respond(YO_NEW_REF ::yield::http::HttpResponse& response);

  /**
    Respond to the HTTP request with a chunked body.
    respond(HttpResponse&) or respond(status_code) must be called first.
    @param chunk response body chunk
  */
  void respond(YO_NEW_REF ::yield::http::HttpMessageBodyChunk& chunk);

  /**
    Respond to the HTTP request.
    Steals the reference to http_response, which should not be modified
      after this method is called.
    @param http_response HTTP response
  */
  void respond(YO_NEW_REF ::yield::http::server::HttpConnection& http_response);

  /**
    Respond to the HTTP request.
    @param status_code response status code e.g, 200
  */
  void respond(uint16_t status_code);

  /**
    Respond to the HTTP request.
    This method should only be called once.
    @param status_code response status code e.g., 200
    @param body response body
  */
  void respond(uint16_t status_code, YO_NEW_REF Buffer* body);

  /**
    Respond to the HTTP request.
    This method should only be called once.
    @param status_code response status code e.g., 200
    @param body response body
  */
  void respond(uint16_t status_code, YO_NEW_REF Buffer& body);

  /**
    Respond to the HTTP request.
    This method should only be called once.
    @param status_code response status code e.g., 200
    @param body response body
  */
  void respond(uint16_t status_code, const char* body);

  /**
    Respond to the HTTP request.
    This method should only be called once.
    @param status_code response status code e.g., 200
    @param body response body
  */
  void respond(uint16_t status_code, const Exception& body);

public:
  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }

  HttpRequest& inc_ref() {
    return Object::inc_ref(*this);
  }

protected:
  friend class HttpRequestParser;

  HttpRequest(
    YO_NEW_REF Buffer* body,
    HttpConnection& connection,
    uint16_t fields_offset,
    Buffer& header,
    uint8_t http_version,
    Method method,
    const yield::uri::Uri& uri
  );

private:
  HttpConnection& connection_;
  DateTime creation_date_time_;
};
}
}
}

#endif
