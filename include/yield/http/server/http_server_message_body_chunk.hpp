// yield/http/server/http_message_body_chunk.hpp

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

#ifndef _YIELD_HTTP_SERVER_HTTP_SERVER_MESSAGE_BODY_CHUNK_HPP_
#define _YIELD_HTTP_SERVER_HTTP_SERVER_MESSAGE_BODY_CHUNK_HPP_

#include "yield/http/http_message_body_chunk.hpp"

namespace yield {
namespace sockets {
class SocketAddress;
}

namespace http {
namespace server {
class HttpServerConnection;

/**
  A <code>yield::http::HttpMessageBodyChunk</code> that's bound to a server
    <code>HttpConnection</code>.
  These <code>HttpMessageBodyChunk</code>s are usually created by
    <code>HttpRequestQueue</code> as part of a request;
    response <code>HTTPMessageBodyChunks</code> can be the normal
      <code>yield::http::HttpMessageBodyChunk</code>s.
*/
class HttpServerMessageBodyChunk final : public ::yield::http::HttpMessageBodyChunk {
public:
  /**
    Construct an HttpMessageBodyChunk that originates from the given
      server connection.
    @param connection the server connection
    @param data the chunk data
  */
  HttpServerMessageBodyChunk(::std::shared_ptr<HttpServerConnection> connection, ::std::shared_ptr<Buffer> data)
    : ::yield::http::HttpMessageBodyChunk(data),
      connection_(connection) {
  }

public:
  /**
    Get the server connection from which this chunk originated.
    @return the server connection from which this chunk originated
  */
  const HttpServerConnection& connection() const {
    return *connection_;
  }

private:
  ::std::shared_ptr<HttpServerConnection> connection_;
};
}
}
}

#endif
