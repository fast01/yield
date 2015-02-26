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

#ifndef _YIELD_SOCKETS_AIO_SOCKET_AIOCB_HPP_
#define _YIELD_SOCKETS_AIO_SOCKET_AIOCB_HPP_

#include "yield/socket_t.hpp"
#include "yield/ssize_t.hpp"

namespace yield {
namespace sockets {
class Socket;

namespace aio {
#ifdef _WIN32
namespace win32 {
class Win32SocketAioQueue;
}
#endif

/**
  Asynchronous Input/Output Control Block (SocketAiocb) for sockets.
*/
class SocketAiocb {
public:
  enum class Type {
    ACCEPT,
    CONNECT,
    RECV,
    RECVFROM,
    SEND,
    SENDFILE
  };

public:
  virtual ~SocketAiocb() {
  }

public:
  /**
    Get the error code of the AIO operation associated with this control block,
      once the operation is complete.
    Callers should first check that get_return() < 0.
    Similar to POSIX aio_error.
    @return the error code produced by the operation associated with this
      control block
  */
  uint32_t error() const {
    return error_;
  }

  /**
   * Get the offset of the AIO operation associated with this control block.
   */
  off_t offset() const {
    return offset_;
  }

  /**
    Get the return value of the AIO operation associated with this control block,
      once the operation is complete.
    Similar to POSIX aio_return.
    @return the return value produced by the operation associated with this
      control block
  */
  ssize_t return_() const {
    return return__;
  }

  virtual Socket& socket() = 0;

  virtual Type type() const = 0;

protected:
  SocketAiocb() {
    init(0);
  }

  SocketAiocb(off_t offset) {
    init(offset);
  }

protected:
  friend class SocketNbioQueue;
#ifdef _WIN32
  friend class win32::Win32SocketAioQueue;
#endif

  void set_error(uint32_t error) {
    this->error_ = error;
  }

  void set_return(ssize_t return_) {
    this->return__ = return_;
  }

private:
  void init(off_t offset);

private:
  uint32_t error_;
  off_t offset_;
  ssize_t return__;

#ifdef _WIN32
  struct {
    unsigned long* Internal;
    unsigned long* InternalHigh;
#pragma warning(push)
#pragma warning(disable: 4201)
    union {
      struct {
        unsigned long Offset;
        unsigned long OffsetHigh;
      };
      void* Pointer;
    };
#pragma warning(pop)
    void* hEvent;
  } overlapped_;
  SocketAiocb* this_;
#endif
};
}
}
}

#endif
