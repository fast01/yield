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

#include "yield/sockets/aio/socket_aio_queue.hpp"
#include "yield/logging.hpp"
#include "yield/sockets/aio/accept_aiocb.hpp"
#include "yield/sockets/aio/connect_aiocb.hpp"
#include "yield/sockets/aio/recv_aiocb.hpp"
#include "yield/sockets/aio/recvfrom_aiocb.hpp"
#include "yield/sockets/aio/send_aiocb.hpp"
#include "yield/sockets/aio/sendfile_aiocb.hpp"

#ifdef _WIN32
#include "win32/win32_socket_aio_queue.hpp"
#else
#include "yield/sockets/aio/socket_nbio_queue.hpp"
#endif

namespace yield {
namespace sockets {
namespace aio {
using ::std::unique_ptr;

unique_ptr<SocketAioQueue> SocketAioQueue::create() {
#ifdef _WIN32
  return unique_ptr<SocketAioQueue>(new win32::Win32SocketAioQueue());
#else
  return unique_ptr<SocketAioQueue>(new SocketNbioQueue());
#endif
}

template <class AiocbType> void SocketAioQueue::log_completion(AiocbType& aiocb) {
  if (aiocb.return_() >= 0) {
    DLOG(DEBUG) << "completed " << aiocb;
  } else {
    log_error(aiocb);
  }
}

template <class AiocbType> void SocketAioQueue::log_enqueue(AiocbType& aiocb) {
  DLOG(DEBUG) << "enqueuing " << aiocb;
}

template <class AiocbType> void SocketAioQueue::log_error(AiocbType& aiocb) {
  LOG(ERROR) << "error on " << aiocb;
}

template void SocketAioQueue::log_completion(AcceptAiocb&);
template void SocketAioQueue::log_completion(ConnectAiocb&);
template void SocketAioQueue::log_completion(RecvAiocb&);
template void SocketAioQueue::log_completion(RecvfromAiocb&);
template void SocketAioQueue::log_completion(SendAiocb&);
template void SocketAioQueue::log_completion(SendfileAiocb&);
template void SocketAioQueue::log_enqueue(AcceptAiocb&);
template void SocketAioQueue::log_enqueue(ConnectAiocb&);
template void SocketAioQueue::log_enqueue(RecvAiocb&);
template void SocketAioQueue::log_enqueue(RecvfromAiocb&);
template void SocketAioQueue::log_enqueue(SendAiocb&);
template void SocketAioQueue::log_enqueue(SendfileAiocb&);
template void SocketAioQueue::log_error(AcceptAiocb&);
template void SocketAioQueue::log_error(ConnectAiocb&);
template void SocketAioQueue::log_error(RecvAiocb&);
template void SocketAioQueue::log_error(RecvfromAiocb&);
template void SocketAioQueue::log_error(SendAiocb&);
template void SocketAioQueue::log_error(SendfileAiocb&);
}
}
}
