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

#ifndef _YIELD_SOCKETS_AIO_SOCKET_AIO_QUEUE_HPP_
#define _YIELD_SOCKETS_AIO_SOCKET_AIO_QUEUE_HPP_

#include "yield/event_queue.hpp"
#include "yield/sockets/aio/socket_aiocb.hpp"

namespace yield {
namespace sockets {
namespace aio {
/**
  Queue for asynchronous input/output (AIO) operations on sockets.
  The queue is similar to Win32 I/O completion ports or Linux io_submit AIO:
    AIO operations are described by AIO control blocks (Aiocbs), enqueued
    to the AIO queue, and dequeued on completion.
  The AioQueue also serves as a general thread-safe EventQueue, passing non-SocketAiocb
    events through from producer to consumer.
*/
class SocketAioQueue : public EventQueue<SocketAiocb> {
public:
  virtual ~SocketAioQueue() {
  }

public:
  /**
    Associate a socket with this AioQueue.
    On Win32 it is necessary to call this method once per socket before
      enqueueing an AIO operation/AIO operation on the socket.
  */
  virtual bool associate(socket_t socket_) = 0;

public:
  /**
    Create a platform-specific SocketAioQueue implementation.
  */
  static ::std::unique_ptr<SocketAioQueue> create();

protected:
  template <class AiocbType> void log_completion(AiocbType& aiocb);
  template <class AiocbType> void log_enqueue(AiocbType& aiocb);
  template <class AiocbType> void log_error(AiocbType& aiocb);
};
}
}
}

#endif
