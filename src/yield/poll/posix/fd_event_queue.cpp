// yield/poll/posix/fd_event_queue.cpp

// Copyright (c) 2013 Minor Gordon
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

#include "yield/debug.hpp"
#include "yield/poll/fd_event_queue.hpp"

#include <errno.h>

namespace yield {
namespace poll {
#if !defined(__linux__) && \
    !defined(__MACH__) && \
    !defined(__FreeBSD__) && \
    !defined(__sun)
FdEventQueue::FdEventQueue(bool) throw(Exception) {
  if (pipe(wake_pipe) != -1) {
    try {
      if (!associate(wake_pipe[0], FdEvent::TYPE_READ_READY)) {
        throw Exception();
      }
    } catch (Exception&) {
      close(wake_pipe[0]);
      close(wake_pipe[1]);
      throw;
    }
  } else {
    throw Exception();
  }
}

FdEventQueue::~FdEventQueue() {
  close(wake_pipe[0]);
  close(wake_pipe[1]);
}

bool FdEventQueue::associate(fd_t fd, FdEvent::Type fd_event_types) {
  if (fd_event_types > 0) {
    for (
      vector<pollfd>::iterator pollfd_i = pollfds.begin();
      pollfd_i != pollfds.end();
      ++pollfd_i
    ) {
      if ((*pollfd_i).fd == fd) {
        (*pollfd_i).events = fd_event_types;
        return true;
      }
    }

    pollfd pollfd_;
    memset(&pollfd_, 0, sizeof(pollfd_));
    pollfd_.fd = fd;
    pollfd_.events = fd_event_types;
    pollfds.push_back(pollfd_);
    return true;
  } else {
    return dissociate(fd);
  }
}

bool FdEventQueue::dissociate(fd_t fd) {
  for (
    vector<pollfd>::iterator pollfd_i = pollfds.begin();
    pollfd_i != pollfds.end();
    ++pollfd_i
  ) {
    if ((*pollfd_i).fd == fd) {
      pollfds.erase(pollfd_i);
      return true;
    }
  }

  errno = ENOENT;
  return false;
}

bool FdEventQueue::enqueue(YO_NEW_REF Event& event) {
  if (event_queue.enqueue(event)) {
    ssize_t write_ret = write(wake_pipe[1], "m", 1);
    debug_assert_eq(write_ret, 1);
    return true;
  } else {
    return false;
  }
}

YO_NEW_REF Event* FdEventQueue::timeddequeue(const Time& timeout) {
  int timeout_ms
  = (timeout == Time::FOREVER) ? -1 : static_cast<int>(timeout.ms());
  int ret = ::poll(&pollfds[0], pollfds.size(), timeout_ms);
  if (ret > 0) {
    vector<pollfd>::const_iterator pollfd_i = pollfds.begin();

    do {
      const pollfd& pollfd_ = *pollfd_i;

      if (pollfd_.revents != 0) {
        if (pollfd_.fd == wake_pipe[0]) {
          char data;
          read(wake_pipe[0], &data, sizeof(data));
          return event_queue.trydequeue();
        } else {
          return new FdEvent(pollfd_.fd, pollfd_.revents);
        }

        if (--ret == 0) {
          break;
        }
      }
    } while (++pollfd_i != pollfds.end());

    debug_break();
    return NULL;
  } else if (ret == 0 || errno == EINTR) {
    return NULL;
  } else {
    debug_break();
    return NULL;
  }
}
#endif
}
}
