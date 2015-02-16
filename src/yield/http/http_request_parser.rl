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

#include "yield/buffer.hpp"
#include "yield/logging.hpp"
#include "yield/http/http_request_parser.hpp"
#include "yield/http/http_response.hpp"

#include <stdlib.h> // For atof and atoi

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4702)
#else
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

namespace yield {
namespace http {
using yield::uri::Uri;

void HttpRequestParser::parse(ParseCallbacks& callbacks) {
  if (p < eof) {
    ps = p;

    uint8_t http_version;
    HttpRequest::Method method;
    iovec uri_fragment = {0, 0};
    iovec uri_host = {0, 0};
    iovec uri_path = {0, 0};
    uint16_t uri_port = 0;
    iovec uri_query = {0, 0};
    iovec uri_scheme = {0, 0};
    iovec uri_userinfo = {0, 0};

    if (
      parse_request_line(
        http_version,
        method,
        uri_host,
        uri_path,
        uri_port,
        uri_query,
        uri_scheme,
        uri_userinfo
      )
    ) {
      Uri uri(
        buffer_,
        uri_fragment,
        uri_host,
        uri_path,
        uri_port,
        uri_query,
        uri_scheme,
        uri_userinfo
      );

      uint16_t fields_offset;
      size_t content_length;
      if (parse_fields(fields_offset, content_length)) {
	    ::std::shared_ptr<Buffer> body;
        if (parse_body(content_length, body)) {
          callbacks.handle_http_request(create_http_request(
                   body,
                   fields_offset,
                   buffer_,
                   http_version,
                   method,
                   uri
                ));
				return;
        } else {
          ::std::shared_ptr<Buffer> next_buffer
          = Buffer::copy(
              Buffer::getpagesize(),
              p - ps + content_length,
              ps,
              eof - ps
            );
          ps = p;
		  callbacks.read(next_buffer);
		  return;
        }
      }
    } else { // cs == request_line_parser_error
      parse_body_chunks(callbacks);
	  return;
    }

    if (p == eof) { // EOF parsing
      ::std::shared_ptr<Buffer> next_buffer
      = Buffer::copy(
          Buffer::getpagesize(),
          eof - ps + Buffer::getpagesize(),
          ps,
          eof - ps
        );
      p = ps;
	  callbacks.read(next_buffer);
    } else { // Error parsing
      ::std::unique_ptr<HttpResponse> http_response
        = ::std::move(create_http_response(NULL, 400, http_version));
      http_response->set_field("Content-Length", 14, "0", 1);
	  callbacks.handle_error_http_response(::std::move(http_response));
    }
  } else { // p == eof
    ::std::shared_ptr<Buffer> next_buffer = ::std::make_shared<Buffer>(Buffer::getpagesize(), Buffer::getpagesize());
	callbacks.read(next_buffer);
  }
}

bool HttpRequestParser::parse_request_line(
  uint8_t& http_version,
  HttpRequest::Method& method,
  iovec& host,
  iovec& path,
  uint16_t& port,
  iovec& query,
  iovec& scheme,
  iovec& userinfo
) {
  int cs;

  %%{
    machine request_line_parser;
    alphtype unsigned char;

    include rfc2616 "rfc2616.rl";
    include rfc3986 "../../../include/yield/uri/rfc3986.rl";

    # RFC 2616 5.1.1
    method
      = ("CONNECT" % { method = HttpRequest::Method::CONNECT; }) |
        ("COPY" % { method = HttpRequest::Method::COPY; }) |
        ("DELETE" % { method = HttpRequest::Method::DELETE; }) |
        ("GET" % { method = HttpRequest::Method::GET; }) |
        ("HEAD" % { method = HttpRequest::Method::HEAD; }) |
        ("LOCK" % { method = HttpRequest::Method::LOCK; }) |
        ("MKCOL" % { method = HttpRequest::Method::MKCOL; }) |
        ("MOVE" % { method = HttpRequest::Method::MOVE; }) |
        ("OPTIONS" % { method = HttpRequest::Method::OPTIONS; }) |
        ("PATCH" % { method = HttpRequest::Method::PATCH; }) |
        ("POST" % { method = HttpRequest::Method::POST; }) |
        ("PROPFIND" % { method = HttpRequest::Method::PROPFIND; }) |
        ("PROPPATCH" % { method = HttpRequest::Method::PROPPATCH; }) |
        ("PUT" % { method = HttpRequest::Method::PUT; }) |
        ("TRACE" % { method = HttpRequest::Method::TRACE; }) |
        ("UNLOCK" % { method = HttpRequest::Method::UNLOCK; });

    # RFC 2616 5.1.2
    # Per the spec, Request-Uri = "*" | absoluteURI | abs_path | authority
    # absoluteURI is fully-qualified (http://host...), for use with proxies.
    # authority is (userinfo "@")? host (":" port)?, used with CONNECT.
    # abs_path is the common form, but it does NOT include a ["?" query],
    #   only path segments.
    # -> an oversight in the spec, corrected here.
    request_uri = (
        (
          '*'
          >{ path.iov_base = p; }
          %{ path.iov_len = p - static_cast<char*>(path.iov_base); }
        ) |
        absolute_uri |
        (path_absolute ("?" query)?) |
        authority
    );

    # RFC 2616 5.1
    request_line = method ' '+ request_uri ' '+ http_version crlf;

    main := request_line
            @{ fbreak; };

    write data;
    write init;
    write exec noend;
  }%%

  return cs != request_line_parser_error;
}
}
}

#ifdef _WIN32
#pragma warning(pop)
#else
#pragma GCC diagnostic warning "-Wold-style-cast"
#pragma GCC diagnostic warning "-Wunused-variable"
#endif
//
