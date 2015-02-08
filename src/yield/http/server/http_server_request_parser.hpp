// yield/http/server/http_request_parser.hpp

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

#ifndef _YIELD_HTTP_SERVER_HTTP_SERVER_REQUEST_PARSER_HPP_
#define _YIELD_HTTP_SERVER_HTTP_SERVER_REQUEST_PARSER_HPP_

#include "yield/http/http_request_parser.hpp"
#include "yield/http/server/http_server_connection.hpp"
#include "yield/http/server/http_server_request.hpp"
#include "yield/http/server/http_server_message_body_chunk.hpp"

namespace yield {
namespace sockets {
class SocketAddress;
}

namespace http {
namespace server {
class HttpServerRequestParser : public ::yield::http::HttpRequestParser {
public:
  HttpServerRequestParser(::std::shared_ptr<HttpServerConnection> connection, ::std::shared_ptr<Buffer> data)
    : HttpRequestParser(data),
      connection_(connection) {
  }

protected:
  // yield::http::HttpMessageParser
  virtual ::std::unique_ptr<HttpMessageBodyChunk>
  create_http_message_body_chunk(
    ::std::shared_ptr<Buffer> data
  ) override {
    return ::std::unique_ptr<HttpMessageBodyChunk>(new HttpServerMessageBodyChunk(connection_, data));
  }

  // yield::http::HttpRequestParser
  ::std::unique_ptr<HttpRequest>
  create_http_request(
    ::std::shared_ptr<Buffer> body,
    uint16_t fields_offset,
    ::std::shared_ptr<Buffer> header,
    uint8_t http_version,
    HttpRequest::Method method,
    const yield::uri::Uri& uri
  ) override {
  return ::std::unique_ptr<HttpRequest>(new HttpServerRequest(
           body,
           connection_,
           fields_offset,
           header,
           http_version,
           method,
           uri
         ));
  }

private:
  ::std::shared_ptr<HttpServerConnection> connection_;
};
}
}
}

#endif
