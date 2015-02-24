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

#include "watches.hpp"
#include "yield/exception.hpp"
#include "yield/logging.hpp"
#include "yield/fs/poll/fs_event_queue.hpp"

#include <errno.h>
#include <limits.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/inotify.h>
#include <unistd.h>

namespace yield {
namespace fs {
namespace poll {
using linux::Watches;

FsEventQueue::FsEventQueue() {
  epoll_fd_ = ::epoll_create(32768);
  if (epoll_fd_ == -1) {
    throw Exception();
  }

  event_fd_ = ::eventfd(0, 0);
  if (event_fd_ == -1) {
    Exception e;
    ::close(epoll_fd_);
    throw e;
  }

  epoll_event epoll_event_;
  memset(&epoll_event_, 0, sizeof(epoll_event_));
  epoll_event_.data.fd = event_fd;
  epoll_event_.events = EPOLLIN;
  if (::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, event_fd_, &epoll_event_) != 0) {
    Exception e;
    ::close(epoll_fd_);
    ::close(event_fd_);
    throw e;
  }

  inotify_fd_ = inotify_init();
  if (inotify_fd_ == -1) {
    Exception e;
    ::close(epoll_fd_);
    ::close(event_fd_);
    throw e;
  }

  memset(&epoll_event_, 0, sizeof(epoll_event_));
  epoll_event_.data.fd = inotify_fd_;
  epoll_event_.events = EPOLLIN;
  if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, inotify_fd_, &epoll_event_) != 0) {
    Exception e;
    ::close(epoll_fd_);
    ::close(event_fd_);
    ::close(inotify_fd_);
    throw e;
  }

  watches_ = new linux::Watches;
}

FsEventQueue::~FsEventQueue() {
  ::close(epoll_fd_);
  ::close(event_fd_);
  ::close(inotify_fd);
  delete watches_;
}

bool
FsEventQueue::associate(
  const Path& path,
  FsEvent::Type fs_event_types
) {
  linux::Watch* watch = watches_->find(path);
  if (watch != NULL) {
    if (watch->fs_event_types() == fs_event_types) {
      return true;
    } else {
      watch = watches_->erase(path);
      CHECK_NOTNULL(watch);
      delete watch;
    }
  }

  uint32_t mask = 0;
  if (fs_event_types & FsEvent::TYPE_DIRECTORY_ADD) {
    mask |= IN_CREATE;
  }
  if (fs_event_types & FsEvent::TYPE_DIRECTORY_MODIFY) {
    mask |= IN_ATTRIB | IN_MODIFY;
  }
  if (fs_event_types & FsEvent::TYPE_DIRECTORY_REMOVE) {
    mask |= IN_DELETE | IN_DELETE_SELF;
  }
  if (fs_event_types & FsEvent::TYPE_DIRECTORY_RENAME) {
    mask |= IN_MOVE_SELF | IN_MOVED_FROM | IN_MOVED_TO;
  }
  if (fs_event_types & FsEvent::TYPE_FILE_ADD) {
    mask |= IN_CREATE;
  }
  if (fs_event_types & FsEvent::TYPE_FILE_MODIFY) {
    mask |= IN_ATTRIB | IN_MODIFY;
  }
  if (fs_event_types & FsEvent::TYPE_FILE_REMOVE) {
    mask |= IN_DELETE | IN_DELETE_SELF;
  }
  if (fs_event_types & FsEvent::TYPE_FILE_RENAME) {
    mask |= IN_MOVE_SELF | IN_MOVED_FROM | IN_MOVED_TO;
  }

  int wd = inotify_add_watch(inotify_fd, path.c_str(), mask);
  if (wd != -1) {
    watch = new linux::Watch(fs_event_types, inotify_fd, path, wd);
    watches->insert(*watch);
    return true;
  } else {
    return false;
  }
}

bool FsEventQueue::dissociate(const Path& path) {
  linux::Watch* watch = watches_->erase(path);
  if (watch != NULL) {
    delete watch;
    return true;
  } else {
    return false;
  }
}

::std::unique_ptr<FsEvent> FsEventQueue::tryenqueue(::std::unique_ptr<FsEvent> event) {
  CHECK(false);
  return false;
  //if (event_queue.enqueue(event)) {
  //  uint64_t data = 1;
  //  ssize_t write_ret = write(event_fd, &data, sizeof(data));
  //  CHECK_EQ(write_ret, static_cast<ssize_t>(sizeof(data)));
  //  return true;
  //} else {
  //  return false;
  //}
}

::std::unique_ptr<FsEvent> FsEventQueue::timeddequeue(const Time& timeout) {
  ::std::unique_ptr<FsEvent> event = event_queue_.trydequeue();
  if (event) {
    return event;
  }

  epoll_event epoll_event_;
  int timeout_ms
  = (timeout == Time::FOREVER) ? -1 : static_cast<int>(timeout.ms());
  int ret = epoll_wait(epoll_fd_, &epoll_event_, 1, timeout_ms);

  if (ret == 0) {
    return NULL;
  } else if (ret < 0) {
    CHECK(errno == EINTR);
    return NULL;    
  }

  CHECK_EQ(ret, 1);

  if (epoll_event_.data.fd == event_fd_) {
    uint64_t data;
    ::read(event_fd_, &data, sizeof(data));
    return NULL;
  }

  CHECK_EQ(epoll_event_.data.fd, inotify_fd_);

  char inotify_events[(sizeof(inotify_event) + PATH_MAX) * 16];
  ssize_t read_ret
  = ::read(inotify_fd_, inotify_events, sizeof(inotify_events));
  CHECK_GT(read_ret, 0);

  const char* inotify_events_p = inotify_events;
  const char* inotify_events_pe
  = inotify_events + static_cast<size_t>(read_ret);

  do {
    const inotify_event* inotify_event_
    = reinterpret_cast<const inotify_event*>(inotify_events_p);

    linux::Watch* watch = watches_->find(inotify_event_->wd);
    if (watch != NULL) {
      FsEvent* fs_event = watch->parse(*inotify_event_);
      if (fs_event != NULL) {
        event_queue_.tryenqueue(*fs_event);
      }
    }

    inotify_events_p += sizeof(inotify_event) + inotify_event_->len;
  } while (inotify_events_p < inotify_events_pe);

  return event_queue_.trydequeue();
}

void FsEventQueue::wake() {
  uint64_t data = 1;
  ssize_t write_ret = ::write(event_fd_, &data, sizeof(data));
  CHECK_EQ(write_ret, static_cast<ssize_t>(sizeof(data)));
}
}
}
}
