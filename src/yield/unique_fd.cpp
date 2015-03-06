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

#include "yield/unique_fd.hpp"

#include <memory>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace yield {
bool unique_fd::close() {
  if (fd_ == INVALID_FD) {
    return false;
  }

#ifdef _WIN32
  if (::CloseHandle(fd_)) {
#else
  if (::close(fd_) == 0) {
#endif
    fd_ = INVALID_FD;
    return true;
  } else {
    fd_ = INVALID_FD;
    return false;
  }
}

unique_fd unique_fd::dup() {
#ifdef _WIN32
  fd_t dup_fd;
  if (
    DuplicateHandle(
      GetCurrentProcess(),
      fd_,
      GetCurrentProcess(),
      &dup_fd,
      0,
      FALSE,
      DUPLICATE_SAME_ACCESS
    )
  ) {
    return unique_fd(dup_fd);
  }
#else
  fd_t dup_fd = ::dup(fd_);
  if (dup_fd != INVALID_FD) {
    return unique_fd(dup_fd);
  }
#endif
  return unique_fd();
}
}
