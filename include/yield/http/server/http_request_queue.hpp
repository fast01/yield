// yield/http/server/http_request_queue.hpp

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

#ifndef _YIELD_HTTP_SERVER_HTTP_REQUEST_QUEUE_HPP_
#define _YIELD_HTTP_SERVER_HTTP_REQUEST_QUEUE_HPP_

#include <vector>

#include "yield/exception.hpp"
#include "yield/sockets/socket_address.hpp"
#include "yield/sockets/aio/aio_queue.hpp"

namespace yield {
namespace sockets {
namespace aio {
class AcceptAiocb;
}
class SocketAddress;
class TcpSocket;
}

namespace http {
namespace server {
class HttpConnection;

/**
  An EventQueue that acts as an HTTP server: listening to an (interface, port),
    accepting connections, reading request data from connections, parsing HTTP
    requests, serializing HTTP responses, writing response data to connections.

  HttpRequestQueue is "passive" in that it relies on the caller to drive it.
  HttpRequestQueue::dequeue or its variants must be called repeatedly and frequently
    in order for the server to make progress.
*/
template <class AioQueueType = yield::sockets::aio::AioQueue>
class HttpRequestQueue : public EventQueue {
public:
  /**
    Construct an HttpRequestQueue that listens for connections on the given
      socket address.
    @param sockname address to listen to
  */
  HttpRequestQueue(
    const yield::sockets::SocketAddress& sockname
  ) throw(Exception);

  /**
    Construct an HttpRequestQueue around an extant TCP socket, which will
      then be bound and listen to the given socket address.
    @param socket_ server socket to use
    @param sockname address to bind and listen to
  */
  HttpRequestQueue(
    YO_NEW_REF yield::sockets::TcpSocket& socket_,
    const yield::sockets::SocketAddress& sockname
  ) throw(Exception);

  ~HttpRequestQueue();

public:
  // yield::EventQueue
  YO_NEW_REF Event& dequeue() {
    return EventQueue::dequeue();
  }

  bool enqueue(YO_NEW_REF Event&);
  YO_NEW_REF Event* timeddequeue(const Time& timeout);

private:
  void handle(YO_NEW_REF yield::sockets::aio::AcceptAiocb& accept_aiocb);
  template <class AiocbType> void handle(YO_NEW_REF AiocbType& aiocb);
  void init(const yield::sockets::SocketAddress& sockname) throw(Exception);

private:
  AioQueueType& aio_queue;
  ::std::vector<HttpConnection*> connections;
  yield::sockets::TcpSocket& socket_;
};
}
}
}

#endif
