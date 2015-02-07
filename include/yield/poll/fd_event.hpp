// yield/poll/fd_event.hpp

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

#ifndef _YIELD_POLL_FD_EVENT_HPP_
#define _YIELD_POLL_FD_EVENT_HPP_

#include "yield/types.hpp"

namespace yield {
namespace poll {
/**
  File descriptor event (read and write readiness, errors)
*/
class FdEvent {
public:
  typedef uint16_t Type;
  const static Type TYPE_ERROR;
  const static Type TYPE_HUP;
  const static Type TYPE_READ_READY;
  const static Type TYPE_WRITE_READY;

public:
  /**
    Construct an FdEvent from a file descriptor and a Type.
  */
  FdEvent(fd_t fd, Type type);

public:
  /**
    Get the file descriptor associated with this FdEvent.
    @return the file descriptor associated with this FdEvent
  */
  fd_t fd() const {
    return fd_;
  }

  /**
    Get the Type of this FdEvent.
    @return the Type of this FdEvent
  */
  Type type() const {
    return type_;
  }

private:
  fd_t fd_;
  Type type_;
};
}
}

#endif
