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

#ifndef _YIELD_UNIQUE_FD_HPP_
#define _YIELD_UNIQUE_FD_HPP_

#include "yield/fd_t.hpp"

namespace yield {
class unique_fd final {
public:
  explicit unique_fd(fd_t fd = INVALID_FD)
    : fd_(fd) {
  }

  unique_fd(unique_fd&& other) {
    fd_ = other.fd_;
    other.fd_ = INVALID_FD;
  }

  unique_fd(const unique_fd&) = delete;

  ~unique_fd() {
    close();
  }

public:
  bool close();

  unique_fd dup();

  fd_t get() const {
    return fd_;
  }

public:
  operator bool() const {
    return fd_ != INVALID_FD;
  }

  fd_t& operator*() {
    return fd_;
  }

  const fd_t& operator*() const {
    return fd_;
  }

  const fd_t* operator->() const {
    return &fd_;
  }

  fd_t* operator->() {
    return &fd_;
  }

  unique_fd& operator=(unique_fd&& other) {
    fd_ = other.fd_;
    other.fd_ = INVALID_FD;
    return *this;
  }

  unique_fd& operator=(const unique_fd&) = delete;

public:
  fd_t release() {
    fd_t ret = get();
    fd_ = INVALID_FD;
    return ret;
  }

  void reset(fd_t fd) {
    close();
    fd_ = fd;
  }

private:
  fd_t fd_;
};
}

#endif
