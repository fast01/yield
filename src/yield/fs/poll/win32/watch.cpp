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

#include "watch.hpp"
#include "yield/logging.hpp"
#include "yield/event_handler.hpp"
#include "yield/fs/file_system.hpp"

#include <Windows.h>

namespace yield {
namespace fs {
namespace poll {
namespace win32 {
using ::std::move;
using ::std::shared_ptr;
using ::std::unique_ptr;

Watch::Watch(
  shared_ptr<Directory> directory,
  FsEvent::Type fs_event_types,
  const Path& path
) : yield::fs::poll::Watch(fs_event_types, path),
  directory_(move(directory)) {
  static_assert(sizeof(overlapped_) == sizeof(::OVERLAPPED), "");
  memset(&overlapped_, 0, sizeof(overlapped_));
  this_ = this;

  notify_filter_ = 0;

  if (
    fs_event_types & FsEvent::TYPE_DIRECTORY_ADD
    ||
    fs_event_types & FsEvent::TYPE_DIRECTORY_REMOVE
    ||
    fs_event_types & FsEvent::TYPE_DIRECTORY_RENAME
  ) {
    notify_filter_ |= FILE_NOTIFY_CHANGE_DIR_NAME;
  }

  if (
    fs_event_types & FsEvent::TYPE_DIRECTORY_MODIFY
    ||
    fs_event_types & FsEvent::TYPE_FILE_MODIFY
  ) {
    notify_filter_ |= FILE_NOTIFY_CHANGE_ATTRIBUTES |
                     FILE_NOTIFY_CHANGE_CREATION |
                     FILE_NOTIFY_CHANGE_LAST_ACCESS |
                     FILE_NOTIFY_CHANGE_LAST_WRITE |
                     FILE_NOTIFY_CHANGE_SIZE;
  }

  if (
    fs_event_types & FsEvent::TYPE_FILE_ADD
    ||
    fs_event_types & FsEvent::TYPE_FILE_REMOVE
    ||
    fs_event_types & FsEvent::TYPE_FILE_RENAME
  ) {
    notify_filter_ |= FILE_NOTIFY_CHANGE_FILE_NAME;
  }
}

Watch::~Watch() {
  close();
}

Watch& Watch::cast(::OVERLAPPED& lpOverlapped) {
  Watch* directory_watch;

  memcpy_s(
    &directory_watch,
    sizeof(directory_watch),
    reinterpret_cast<char*>(&lpOverlapped) + sizeof(::OVERLAPPED),
    sizeof(directory_watch)
  );

  return *directory_watch;
}

void Watch::close() {
  if (directory_ != NULL) {
    CancelIoEx(*directory_, *this);
    directory_->close();
    directory_.reset();
  }
}

bool Watch::is_closed() const {
  return directory_ == NULL;
}

void Watch::log_read(const FILE_NOTIFY_INFORMATION& file_notify_info) {
  DLOG(DEBUG) <<
      "yield::fs::poll::win32::Watch(" <<
      "path=" << path() <<
      ")" <<
      ": read FILE_NOTIFY_INFORMATION(" <<
      "Action=" << file_notify_info.Action <<
      ", "
      "FileName=" <<
      Path(
        file_notify_info.FileName,
        file_notify_info.FileNameLength / sizeof(wchar_t)
      ) <<
      ")";
}

Watch::operator ::OVERLAPPED* () {
  return reinterpret_cast< ::OVERLAPPED* >(&overlapped_);
}
}
}
}
}
