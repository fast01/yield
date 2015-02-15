// yield/sockets/aio/nbio_queue.hpp

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

#ifndef _YIELD_SOCKETS_AIO_SOCKET_NBIO_QUEUE_HPP_
#define _YIELD_SOCKETS_AIO_SOCKET_NBIO_QUEUE_HPP_

#include "yield/event_queue.hpp"
#include "yield/poll/fd_event_queue.hpp"
#include "yield/queue/blocking_concurrent_queue.hpp"
#include "yield/sockets/aio/socket_aiocb.hpp"
#include "yield/sockets/aio/socket_aio_queue.hpp"

#include <map>

namespace yield {
class Buffer;

namespace sockets {
namespace aio {
class AcceptAiocb;
class ConnectAiocb;
class RecvAiocb;
class RecvfromAiocb;
class SendAiocb;
class SendfileAiocb;

/**
  Queue for asynchronous input/output (AIO) operations on sockets,
    implemented in terms of non-blocking I/O.
  The SocketNbioQueue presents the same interface and emulates the semantics of
    AioQueue for platforms that do not natively support socket AIO (e.g., all
    platforms besides Win32). Even on Win32 the SocketNbioQueue is needed for socket
    types that only support blocking or non-blocking I/O, like SSL sockets.
  @see AioQueue
*/

class SocketNbioQueue final : public SocketAioQueue {
public:
  /**
    Construct a SocketNbioQueue.
  */
  SocketNbioQueue();

public:
  /**
    Associate a socket with this AioQueue.
    This is a no-op in the SocketNbioQueue, intended only to conform to the interface
      of the AioQueue on Win32.
  */
  bool associate(socket_t) {
    return true;
  }

public:
  // yield::EventQueue
  ::std::unique_ptr<SocketAiocb> timeddequeue(const Time& timeout) override;
  ::std::unique_ptr<SocketAiocb> tryenqueue(::std::unique_ptr<SocketAiocb> aiocb) override;
  void wake() override;

private:
  class AiocbState;

  enum RetryStatus {
    RETRY_STATUS_COMPLETE,
    RETRY_STATUS_ERROR,
    RETRY_STATUS_WANT_RECV,
    RETRY_STATUS_WANT_SEND
  };

  class SocketState;

private:
  void associate(SocketAiocb& aiocb, RetryStatus retry_status);

private:
  template <class AiocbType> void log_retry(AiocbType&);
  template <class AiocbType> void log_partial_send(AiocbType&, size_t);
  template <class AiocbType> void log_wouldblock(AiocbType&, RetryStatus);

private:
  static uint8_t get_aiocb_priority(const SocketAiocb& aiocb);

private:
  RetryStatus retry(SocketAiocb&, size_t& partial_send_len);
  RetryStatus retry_accept(AcceptAiocb&);
  RetryStatus retry_connect(ConnectAiocb&, size_t& partial_send_len);
  RetryStatus retry_recv(RecvAiocb&);
  RetryStatus retry_recvfrom(RecvfromAiocb&);
  RetryStatus retry_send(SendAiocb&, size_t& partial_send_len);
  template <class AiocbType>
  RetryStatus retry_send(AiocbType&, const Buffer&, size_t& partial_send_len);
  RetryStatus retry_sendfile(SendfileAiocb&, size_t& partial_send_len);

private:
  ::yield::queue::BlockingConcurrentQueue<SocketAiocb> aiocb_queue;
  ::yield::poll::FdEventQueue fd_event_queue;
  ::std::map<fd_t, SocketState*> socket_state;
};
}
}
}

#endif
