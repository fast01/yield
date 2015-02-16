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

#ifndef _YIELD_FS_POLL_SCANNING_FS_EVENT_QUEUE_HPP_
#define _YIELD_FS_POLL_SCANNING_FS_EVENT_QUEUE_HPP_

#include <map>

#include "yield/event_queue.hpp"
#include "yield/fs/poll/fs_event.hpp"
#include "yield/stage/synchronized_event_queue.hpp"

namespace yield {
namespace fs {
namespace poll {
class ScanningWatch;
template <class> class Watches;

/**
  An FsEventQueue implementation that periodically scans a file system
    (i.e., FileSystem implementations) for changes.
  Used as a fallback implementation when more efficient means (such as
    ReadDirectoryChangesW on Win32 or inotify on Linux) are not available.
*/
class ScanningFsEventQueue final : public EventQueue<FsEvent> {
public:
  /**
    Associate a file system path with this ScanningFsEventQueue,
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
    Dissociate a file system path from this ScanningFsEventQueue.
    @param path the path to dissociate
    @return true on success, false+errno on failure
  */
  bool dissociate(const Path& path);

public:
  // yield::EventQueue
  ::std::unique_ptr<FsEvent> timeddequeue(const Time& timeout) override;
  ::std::unique_ptr<FsEvent> tryenqueue(::std::unique_ptr<FsEvent> event) override;
  void wake() override;

private:
  ::yield::stage::SynchronizedEventQueue<FsEvent> event_queue_;
  ::std::map< Path, ::std::unique_ptr<ScanningWatch> > watches_;
};
}
}
}

#endif
