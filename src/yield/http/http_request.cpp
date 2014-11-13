// yield/http/http_request.cpp

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
#include "yield/exception.hpp"
#include "yield/logging.hpp"
#include "yield/http/http_request.hpp"

#include <sstream> // for std::ostringstream

namespace yield {
namespace http {
HttpRequest::Method HttpRequest::Method::CONNECT(1, "CONNECT", 7);
HttpRequest::Method HttpRequest::Method::COPY(2, "COPY", 4);
#ifdef DELETE
#undef DELETE
#endif
HttpRequest::Method HttpRequest::Method::DELETE(3, "DELETE", 7);
HttpRequest::Method HttpRequest::Method::GET(4, "GET", 3);
HttpRequest::Method HttpRequest::Method::HEAD(5, "HEAD", 4);
HttpRequest::Method HttpRequest::Method::LOCK(6, "LOCK", 4);
HttpRequest::Method HttpRequest::Method::MKCOL(7, "MKCOL", 5);
HttpRequest::Method HttpRequest::Method::MOVE(8, "MOVE", 4);
HttpRequest::Method HttpRequest::Method::OPTIONS(9, "OPTIONS", 7);
HttpRequest::Method HttpRequest::Method::PATCH(10, "PATCH", 5);
HttpRequest::Method HttpRequest::Method::POST(11, "POST", 4);
HttpRequest::Method HttpRequest::Method::PROPFIND(12, "PROPFIND", 8);
HttpRequest::Method HttpRequest::Method::PROPPATCH(13, "PROPPATCH", 9);
HttpRequest::Method HttpRequest::Method::PUT(14, "PUT", 3);
HttpRequest::Method HttpRequest::Method::TRACE(15, "TRACE", 5);
HttpRequest::Method HttpRequest::Method::UNLOCK(16, "UNLOCK", 6);

HttpRequest::Method
HttpRequest::Method::parse(
  const char* method
) throw(Exception) {
  if (method != NULL) {
    switch (method[0]) {
    case 'D':
      return DELETE;
    case 'G':
      return GET;
    case 'H':
      return HEAD;
    case 'M': {
      switch (method[1]) {
      case 'K':
        return MKCOL;
      case 'O':
        return MOVE;
      }
    }
    break;
    case 'O':
      return OPTIONS;
    case 'P': {
      switch (method[1]) {
      case 'A':
        return PATCH;
      case 'O':
        return POST;
      case 'R': {
        if (method[2] == 'O' && method[3] == 'P') {
          switch (method[3]) {
          case 'F':
            return PROPFIND;
          case 'P':
            return PROPPATCH;
          }
        }
      }
      break;
      case 'U':
        return PUT;
      }
    }
    break;
    case 'T':
      return TRACE;
    case 'U':
      return UNLOCK;
    }
  }

  throw Exception("unknown method");
}

HttpRequest::HttpRequest(
  YO_NEW_REF Object* body,
  uint16_t fields_offset,
  Buffer& header,
  uint8_t http_version,
  Method method,
  const yield::uri::Uri& uri
)
  : HttpMessage<HttpRequest>(
    body,
    fields_offset,
    header,
    http_version
  ),
  method(method),
  uri(uri)
{ }

HttpRequest::HttpRequest(
  Method method,
  const yield::uri::Uri& uri,
  YO_NEW_REF Object* body,
  uint8_t http_version
)
  : HttpMessage<HttpRequest>(body, http_version),
    method(method),
    uri(uri) {
  get_header().put(method.get_name(), method.get_name_len());

  get_header().put(' ');

  iovec uri_path;
  uri.get_path(uri_path);
  get_header().put(uri_path);

  if (http_version == 0) {
    get_header().put(" HTTP/1.0\r\n", 11);
  } else {
    get_header().put(" HTTP/1.1\r\n", 11);
  }

  set_fields_offset(static_cast<uint16_t>(get_header().size()));

  if (uri.has_host()) {
    iovec uri_host;
    uri.get_host(uri_host);
    if (uri.get_port() == 80) {
      set_field("Host", 4, uri_host);
    } else {
      const char* uri_port_p
      = static_cast<char*>(uri_host.iov_base) + uri_host.iov_len;
      if (
        uri_port_p >= static_cast<char*>(uri_path.iov_base) - 6
        &&
        uri_port_p < uri_path.iov_base
        &&
        *uri_port_p == ':'
      ) {
        const char* uri_port_ps = uri_port_p;
        uri_port_p++;
        while (uri_port_p < uri_path.iov_base && isdigit(*uri_port_p)) {
          uri_port_p++;
        }
        uri_host.iov_len += uri_port_p - uri_port_ps;

        set_field("Host", 4, uri_host);
      } else {
        std::ostringstream host;
        host.write(
          static_cast<char*>(uri_host.iov_base),
          uri_host.iov_len
        );
        host << ':';
        host << uri.get_port();

        set_field("Host", 4, host.str());
      }
    }
  }
}

std::ostream& operator<<(std::ostream& os, const HttpRequest& http_request) {
  std::ostringstream body;
  if (http_request.get_body() != NULL) {
    if (http_request.get_body()->get_type_id() == Buffer::TYPE_ID) {
      body << static_cast<Buffer*>(http_request.get_body());
    } else {
      body << "Buffer";
    }
  } else {
    body << "NULL";
  }

  os <<
     "HttpRequest(" <<
     "content_length=" << http_request.get_content_length() <<
     ", " <<
     "http_version=" <<
     static_cast<uint16_t>(http_request.get_http_version()) <<
     ", " <<
     "method=" << http_request.get_method() <<
     ", " <<
     "uri=" << http_request.get_uri() <<
     ", " <<
     "body=" << body.str() <<
     ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const HttpRequest::Method& method) {
  os << method.get_name();
  return os;
}
}
}
