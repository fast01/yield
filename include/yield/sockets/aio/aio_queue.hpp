// yield/sockets/aio/aio_queue.hpp

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

#ifndef _YIELD_SOCKETS_AIO_AIO_QUEUE_HPP_
#define _YIELD_SOCKETS_AIO_AIO_QUEUE_HPP_

#ifdef _WIN32
#include "yield/event_queue.hpp"
#include "yield/sockets/aio/aiocb.hpp"
#else
#include "yield/sockets/aio/nbio_queue.hpp"
#endif

namespace yield {
namespace sockets {
namespace aio {
/**
  Queue for asynchronous input/output (AIO) operations on sockets.
  The queue is similar to Win32 I/O completion ports or Linux io_submit AIO:
    AIO operations are described by AIO control blocks (Aiocbs), enqueued
    to the AIO queue, and dequeued on completion.
  The AioQueue also serves as a general thread-safe EventQueue, passing non-Aiocb
    events through from producer to consumer.
*/
#ifdef _WIN32
class AioQueue final : public EventQueue<Aiocb> {
public:
  /**
    Construct an AioQueue.
  */
  AioQueue();

public:
  /**
    Associate a socket with this AioQueue.
    On Win32 it is necessary to call this method once per socket before
      enqueueing an AIO operation/AIO operation on the socket.
  */
  bool associate(socket_t socket_);

public:
  // yield::EventQueue
  bool enqueue(::std::unique_ptr<Aiocb> aiocb) override;
  ::std::unique_ptr<Aiocb> timeddequeue(const Time& timeout) override;
  void wake() override;

private:
  template <class AiocbType> void log_completion(AiocbType& aiocb);
  template <class AiocbType> void log_enqueue(AiocbType& aiocb);
  template <class AiocbType> void log_error(AiocbType& aiocb);

private:
  fd_t hIoCompletionPort;
};
#else
typedef NbioQueue AioQueue;
#endif
}
}
}

#endif
