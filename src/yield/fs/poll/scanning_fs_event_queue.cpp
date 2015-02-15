// yield/fs/poll/scanning_fs_event_queue.cpp

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

#include "./scanning_directory_watch.hpp"
#include "./scanning_file_watch.hpp"
#include "yield/logging.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/poll/scanning_fs_event_queue.hpp"

namespace yield {
namespace fs {
namespace poll {
using ::std::make_pair;
using ::std::move;
using ::std::unique_ptr;

bool
ScanningFsEventQueue::associate(
  const Path& path,
  FsEvent::Type fs_event_types
) {
  dissociate(path);

  unique_ptr<Stat> stbuf = FileSystem().stat(path);
  if (stbuf == NULL) {
    return false;
  }
  unique_ptr<ScanningWatch> watch;
  if (stbuf->ISDIR()) {
    watch.reset(new ScanningDirectoryWatch(fs_event_types, path));
  } else {
    watch.reset(new ScanningFileWatch(fs_event_types, path));
  }
  CHECK(watches_.find(path) == watches_.end());
  watches_.insert(make_pair(path, move(watch)));
  return true;
}

bool ScanningFsEventQueue::dissociate(const Path& path) {
  return watches_.erase(path) == 1;
}

unique_ptr<FsEvent> ScanningFsEventQueue::tryenqueue(unique_ptr<FsEvent> event) {
  return event_queue_.tryenqueue(move(event));
}

unique_ptr<FsEvent> ScanningFsEventQueue::timeddequeue(const Time& timeout) {
  unique_ptr<FsEvent> event = move(event_queue_.trydequeue());
  if (event != NULL) {
    return event;
  }
    
  if (watches_.empty()) {
    return event_queue_.timeddequeue(timeout);
  }

  Time timeout_remaining(timeout);
  for (;;) {
    for (auto watch_i = watches_.cbegin(), end = watches_.cend(); watch_i != end; ++watch_i) {
      Time start_time = Time::now();

      watch_i->second->scan(event_queue_);
      event = event_queue_.trydequeue();
      if (event != NULL) {
        return event;
      }

      Time elapsed_time = Time::now() - start_time;
      if (elapsed_time < timeout_remaining) {
        timeout_remaining -= elapsed_time;
      } else {
        return event_queue_.trydequeue();
      }
    }
  }
}

void ScanningFsEventQueue::wake() {
  event_queue_.wake();
}
}
}
}
