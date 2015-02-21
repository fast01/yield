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

#ifndef _YIELD_HTTP_HTTP_RESPONSE_HPP_
#define _YIELD_HTTP_HTTP_RESPONSE_HPP_

#include "yield/http/http_message.hpp"

namespace yield {
namespace http {
/**
  An RFC 2616 HTTP response.
  Unlike its counterparts in yield.http.client and yield.http.server, this
    HttpResponse is not tied to a particular connection.
*/
class HttpResponse : public HttpMessage<HttpResponse> {
public:
  /**
    Construct an HttpResponse from its constituent parts.
    @param http_version HTTP version as a single byte (0 or 1 for HTTP/1.0 or
      HTTP/1.1, respectively.)
    @param status_code numeric status code e.g., 200
  */
  HttpResponse(
    uint8_t http_version,
    uint16_t status_code
  ) : HttpMessage<HttpResponse>(NULL, NULL, http_version),
    status_code_(status_code) {
    init();
  }

  /**
    Construct an HttpResponse from its constituent parts.
    @param body optional body
    @param http_version HTTP version as a single byte (0 or 1 for HTTP/1.0 or
      HTTP/1.1, respectively.)
    @param status_code numeric status code e.g., 200
  */
  HttpResponse(
    ::std::shared_ptr<Buffer> body,
    uint8_t http_version,
    uint16_t status_code
  ) : HttpMessage<HttpResponse>(body, NULL, http_version),
    status_code_(status_code) {
    init();
  }

  /**
    Construct an HttpResponse from its constituent parts.
    @param body optional body
    @param http_version HTTP version as a single byte (0 or 1 for HTTP/1.0 or
      HTTP/1.1, respectively.)
    @param status_code numeric status code e.g., 200
  */
  HttpResponse(
    ::std::shared_ptr< ::yield::fs::File > body,
    uint8_t http_version,
    uint16_t status_code
  ) : HttpMessage<HttpResponse>(NULL, body, http_version),
    status_code_(status_code) {
    init();
  }

  /**
    Construct an HttpResponse from its constituent parts.
    @param http_version HTTP version as a single byte (0 or 1 for HTTP/1.0 or
      HTTP/1.1, respectively.)
    @param status_code numeric status code e.g., 200
    @param body optional body
  */
  HttpResponse(
    uint8_t http_version,
    uint16_t status_code,
    ::std::shared_ptr<Buffer> body
  ) : HttpMessage<HttpResponse>(body, NULL, http_version),
    status_code_(status_code) {
    init();
  }

  /**
    Construct an HttpResponse from its constituent parts.
    @param body the HTTP request body, optional
    @param fields_offset offset into the header buffer where the fields start
    @param header header buffer, required
    @param http_version the HTTP version as a single byte (0 or 1 for HTTP/1.0
      and HTTP/1.1, respectively)
    @param method the HTTP request method e.g., HttpRequest::Method::GET
    @param uri the HTTP request Uri e.g., /
  */
  HttpResponse(
    ::std::shared_ptr<Buffer> body,
    uint16_t fields_offset,
    ::std::shared_ptr<Buffer> header,
    uint8_t http_version,
    uint16_t status_code
  )  : HttpMessage<HttpResponse>(
        body,
        NULL,
        fields_offset,
        header,
        http_version
      ),
      status_code_(status_code) {
  }

  /**
    Empty virtual destructor.
  */
  virtual ~HttpResponse() { }

public:
  /**
    Get the numeric status code e.g., 200.
    @return the numeric status code
  */
  uint16_t status_code() const {
    return status_code_;
  }

private:
  void init();

private:
  uint16_t status_code_;
};

std::ostream& operator<<(std::ostream&, const HttpResponse&);
}
}

#endif
