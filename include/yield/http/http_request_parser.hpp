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
  /**
    Construct an HttpRequestParser on the underlying buffer.
    References to buffer may be created for any objects (such as HttpRequest)
      produced by the parser.
    @param buffer buffer to parse
  */
  HttpRequestParser(Buffer& buffer)
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
    The caller is responsible for checking the Object's get_type_id
      and downcasting to the appropriate type.
    Object may be of the following types:
    - yield::http::HttpRequest or subclasses: an HTTP request, including its
        body for requests with fixed Content-Length's.
    - yield::Buffer: the next Buffer to read; may include the remainder of
      the previous Buffer that was passed to the constructor
    - yield::http::HttpMessageBodyChunk: a chunk of body data, for requests
        with chunked Transfer-Encoding.
    - yield::http::HttpResponse: an error HTTP response, usually a 400
  */
  YO_NEW_REF Object& parse();

protected:
  virtual YO_NEW_REF HttpRequest&
  create_http_request(
    YO_NEW_REF Buffer* body,
    uint16_t fields_offset,
    Buffer& header,
    uint8_t http_version,
    HttpRequest::Method method,
    const yield::uri::Uri& uri
  ) {
    return *new HttpRequest(
             body,
             fields_offset,
             header,
             http_version,
             method,
             uri
           );
  }

  virtual HttpResponse&
  create_http_response(
    YO_NEW_REF Buffer* body,
    uint8_t http_version,
    uint16_t status_code
  ) {
    return *new HttpResponse(body, http_version, status_code);
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
