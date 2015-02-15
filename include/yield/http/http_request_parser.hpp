// yield/http/http_request_parser.hpp

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

#ifndef _YIELD_HTTP_HTTP_REQUEST_PARSER_HPP_
#define _YIELD_HTTP_HTTP_REQUEST_PARSER_HPP_

#include "yield/http/http_message_parser.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_response.hpp"

namespace yield {
namespace http {
/**
  An RFC 2616-conformant HTTP request parser.
*/
class HttpRequestParser : public HttpMessageParser {
public:
  class ParseCallbacks : public HttpMessageParser::ParseCallbacks {
  public:
    virtual ~ParseCallbacks() {
    }

  public:
    virtual void handle_http_request(::std::unique_ptr<HttpRequest>) = 0;
    virtual void handle_error_http_response(::std::unique_ptr<HttpResponse>) = 0;    
  };

public:
  /**
    Construct an HttpRequestParser on the underlying buffer.
    References to buffer may be created for any objects (such as HttpRequest)
      produced by the parser.
    @param buffer buffer to parse
  */
  HttpRequestParser(::std::shared_ptr<Buffer> buffer)
    : HttpMessageParser(buffer)
  { }

  /**
    Construct an HttpRequestParser on the string buffer.
    Only used for testing.
    @param buffer buffer to parse.
  */
  HttpRequestParser(const ::std::string& buffer)
    : HttpMessageParser(buffer)
  { }

public:
  /**
    Parse the next object from the buffer specified in the constructor.
  */
  void parse(ParseCallbacks&);

protected:
  virtual ::std::unique_ptr<HttpRequest>
  create_http_request(
    ::std::shared_ptr<Buffer> body,
    uint16_t fields_offset,
    ::std::shared_ptr<Buffer> header,
    uint8_t http_version,
    HttpRequest::Method method,
    const yield::uri::Uri& uri
  ) {
    return ::std::unique_ptr<HttpRequest>(new HttpRequest(
             body,
             fields_offset,
             header,
             http_version,
             method,
             uri
           ));
  }

  virtual ::std::unique_ptr<HttpResponse>
  create_http_response(
    ::std::shared_ptr<Buffer> body,
    uint8_t http_version,
    uint16_t status_code
  ) {
    return ::std::unique_ptr<HttpResponse>(new HttpResponse(body, http_version, status_code));
  }

protected:
  bool parse_request_line(
    uint8_t& http_version,
    HttpRequest::Method& method,
    iovec& uri_host,
    iovec& uri_path,
    uint16_t& uri_port,
    iovec& uri_query,
    iovec& uri_scheme,
    iovec& uri_userinfo
  );
};
}
}

#endif
