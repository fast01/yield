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

#include "scanning_file_watch.hpp"
#include "yield/exception.hpp"
#include "yield/event_handler.hpp"
#include "yield/logging.hpp"
#include "yield/fs/file_system.hpp"

namespace yield {
namespace fs {
namespace poll {
using ::std::unique_ptr;

ScanningFileWatch::ScanningFileWatch(
  FsEvent::Type fs_event_types,
  const Path& path
) : ScanningWatch(fs_event_types, path) {
  stbuf = FileSystem().stat(this->path());
  if (stbuf == NULL) {
    throw Exception();
  }
}

void ScanningFileWatch::scan(EventSink<FsEvent>& fs_event_handler) {
  unique_ptr<Stat> new_stbuf = FileSystem().stat(path());
  unique_ptr<Stat> old_stbuf(stbuf.release());
  CHECK_NE(old_stbuf, NULL);

  FsEvent::Type fs_event_type;
  if (new_stbuf != NULL) { // File exists
    if (type(*new_stbuf) == type(*old_stbuf)) { // File has not changed type
      if (equals(*new_stbuf, *old_stbuf)) {
        stbuf.reset(new_stbuf.release());
        return;
      } else {
        fs_event_type = FsEvent::TYPE_FILE_MODIFY;
      }
    } else { // File has changed type
      fs_event_type = FsEvent::TYPE_FILE_REMOVE;
    }
  } else { // File does not exist any longer
    fs_event_type = FsEvent::TYPE_FILE_REMOVE;
  }

  if (want_fs_event_type(fs_event_type)) {
    unique_ptr<FsEvent> fs_event(new FsEvent(path(), fs_event_type));
    log_fs_event(*fs_event);
    fs_event_handler.tryenqueue(move(fs_event));
  }

  stbuf.reset(new_stbuf.release());
}
}
}
}
