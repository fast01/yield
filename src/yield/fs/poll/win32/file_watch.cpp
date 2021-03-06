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

#include "file_watch.hpp"
#include "yield/logging.hpp"
#include "yield/fs/file_system.hpp"

#include <Windows.h>

namespace yield {
namespace fs {
namespace poll {
namespace win32 {
using ::std::unique_ptr;

unique_ptr<FsEvent>
FileWatch::parse(
  const FILE_NOTIFY_INFORMATION& file_notify_info
) {
  log_read(file_notify_info);

  FsEvent::Type fs_event_type;
  Path name(
    file_notify_info.FileName,
    file_notify_info.FileNameLength / sizeof(wchar_t)
  );
  Path path = directory_path_ / name;

  if (path == this->path()) {
    switch (file_notify_info.Action) {
    case FILE_ACTION_MODIFIED: {
      fs_event_type = FsEvent::TYPE_FILE_MODIFY;
    }
    break;

    case FILE_ACTION_REMOVED: {
      fs_event_type = FsEvent::TYPE_FILE_REMOVE;
    }
    break;

    case FILE_ACTION_RENAMED_OLD_NAME: {
      CHECK(old_name_.empty());
      old_name_ = name;
      return unique_ptr<FsEvent>();
    }
    break;

    default:
      CHECK(false);
      return unique_ptr<FsEvent>();
    }

    if (want_fs_event_type(fs_event_type)) {
      unique_ptr<FsEvent> fs_event(new FsEvent(this->path(), fs_event_type));
      log_fs_event(*fs_event);
      return fs_event;
    } else {
      return unique_ptr<FsEvent>();
    }
  } else if (
    !old_name_.empty()
    &&
    file_notify_info.Action == FILE_ACTION_RENAMED_NEW_NAME
  ) {
    CHECK_EQ(directory_path_ / old_name_, this->path());
    fs_event_type = FsEvent::TYPE_FILE_RENAME;
    if (want_fs_event_type(fs_event_type)) {
      unique_ptr<FsEvent> fs_event(new FsEvent(this->path(), path, fs_event_type));
      log_fs_event(*fs_event);
      return fs_event;
    } else {
      return unique_ptr<FsEvent>();
    }
  } else {
    return unique_ptr<FsEvent>();
  }
}
}
}
}
}
