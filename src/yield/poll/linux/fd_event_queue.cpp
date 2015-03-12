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

#include "yield/logging.hpp"
#include "yield/poll/fd_event_queue.hpp"

#include <iostream>
#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

namespace yield {
namespace poll {
using ::std::move;
using ::std::unique_ptr;

FdEventQueue::FdEventQueue(bool) throw(Exception) {
  epfd_ = epoll_create(32768);
  if (epfd_ == -1) {
    throw Exception();
  }

  wake_fd_ = eventfd(0, 0);
  if (wake_fd_ == -1) {
    ::close(epfd_);
    throw Exception();
  }

  if (!associate(wake_fd_, POLLIN)) {
    ::close(wake_fd_);
    ::close(epfd_);
    throw Exception();
  }
}

FdEventQueue::~FdEventQueue() {
  ::close(epfd_);
  ::close(wake_fd_);
}

bool FdEventQueue::associate(fd_t fd, FdEvent::Type fd_event_types) {
  if (fd_event_types > 0) {
    epoll_event epoll_event_;
    memset(&epoll_event_, 0, sizeof(epoll_event_));
    epoll_event_.data.fd = fd;
    epoll_event_.events = fd_event_types;

    if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &epoll_event_) == 0) {
      return true;
    } else if (errno == EEXIST) {
      return epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &epoll_event_) == 0;
    } else {
      return false;
    }
  } else {
    return dissociate(fd);
  }
}

bool FdEventQueue::dissociate(fd_t fd) {
  // From the man page: In kernel versions before 2.6.9,
  // the EPOLL_CTL_DEL operation required a non-NULL pointer in event,
  // even though this argument is ignored. Since kernel 2.6.9,
  // event can be specified as NULL when using EPOLL_CTL_DEL.
  epoll_event epoll_event_;
  memset(&epoll_event_, 0, sizeof(epoll_event_));
  return epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, &epoll_event_) == 0;
}

unique_ptr<FdEvent> FdEventQueue::tryenqueue(unique_ptr<FdEvent>) {
  CHECK(false);
  return unique_ptr<FdEvent>();
/*
  if (event_queue.enqueue(event)) {
    uint64_t data = 1;
    ssize_t write_ret = write(wake_fd, &data, sizeof(data));
    CHECK_EQ(write_ret, static_cast<ssize_t>(sizeof(data)));
    return true;
  } else {
    return false;
  }
*/
}

unique_ptr<FdEvent> FdEventQueue::timeddequeue(const Time& timeout) {
  epoll_event epoll_event_;
  int timeout_ms
    = (timeout == Time::FOREVER) ? -1 : static_cast<int>(timeout.ms());
  int ret = epoll_wait(epfd_, &epoll_event_, 1, timeout_ms);
  if (ret == 0) {
    return unique_ptr<FdEvent>();
  } else if (ret < 0) {
    CHECK_EQ(errno, EINTR);
    return unique_ptr<FdEvent>();
  }
  CHECK_EQ(ret, 1);

  if (epoll_event_.data.fd == wake_fd_) {
    uint64_t data;
    ssize_t read_ret = read(wake_fd_, &data, sizeof(data));
    CHECK_EQ(read_ret, static_cast<ssize_t>(sizeof(data)));
    return unique_ptr<FdEvent>();
  }

  return unique_ptr<FdEvent>(new FdEvent(epoll_event_.data.fd, epoll_event_.events));
}

void FdEventQueue::wake() {
  uint64_t data = 1;
  ssize_t write_ret = write(wake_fd_, &data, sizeof(data));
  CHECK_EQ(write_ret, static_cast<ssize_t>(sizeof(data)));
}
}
}
