// yield/fs/poll/fs_event_queue.hpp

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

#ifndef _YIELD_FS_POLL_FS_EVENT_QUEUE_HPP_
#define _YIELD_FS_POLL_FS_EVENT_QUEUE_HPP_

#if defined(__FreeBSD__) || defined(__linux__) || defined(__MACH__) || defined(_WIN32)
#include "yield/event_queue.hpp"
#include "yield/queue/blocking_concurrent_queue.hpp"
#include "yield/fs/poll/fs_event.hpp"
#else
#include "yield/fs/poll/scanning_fs_event_queue.hpp"
#endif

namespace yield {
namespace fs {
namespace poll {
#if defined(__FreeBSD__) || defined(__linux__) || defined(__MACH__) || defined(_WIN32)
#if defined(__FreeBSD__) || defined(__MACH__)
namespace bsd {
class Watch;
}
#elif defined(__linux__)
namespace linux {
class Watch;
class Watches;
}
#elif defined(_WIN32)
namespace win32 {
class Watch;
}
#endif
template <class> class Watches;

/**
  EventQueue for file system change events (<code>FsEvent</code>s).
*/
class FsEventQueue : public EventQueue<FsEvent> {
public:
  /**
    Construct an FsEventQueue, allocating any system resources as necessary.
  */
  FsEventQueue();

  /**
    Destroy an FdEventQueue, deallocating any associated system resources.
  */
  ~FsEventQueue();

public:
  /**
    Associate a file system path with this FsEventQueue,
      watching for the specified FsEvent types (directory or file
      add/modify/remove/rename).
    @param path the path to associate
    @param fs_event_types FsEvent types to monitor
    @return true on success, false+errno on failure
  */
  bool associate(
    const Path& path,
    FsEvent::Type fs_event_types = FsEvent::TYPE_ALL
  );

  /**
    Dissociate a file system path from this FsEventQueue.
    @param path the path to dissociate
    @return true on success, false+errno on failure
  */
  bool dissociate(const Path& path);

public:
  // yield::EventQueue
  bool enqueue(YO_NEW_REF FsEvent& event);
  YO_NEW_REF FsEvent* timeddequeue(const Time& timeout);

private:
#if defined(__FreeBSD__) || defined(__MACH__)
  ::yield::queue::BlockingConcurrentQueue<Event> event_queue;
  int kq, wake_pipe[2];
  Watches<bsd::Watch>* watches;
#elif defined(__linux__)
  ::yield::queue::BlockingConcurrentQueue<Event> event_queue;
  int epoll_fd, event_fd, inotify_fd;
  linux::Watches* watches;
#elif defined(_WIN32)
  fd_t hIoCompletionPort;
  Watches<win32::Watch>* watches;
#endif
};
#else
typedef ScanningFsEventQueue FsEventQueue;
#endif
}
}
}

#endif
