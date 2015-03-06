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

#include "directory_watch.hpp"
#include "yield/logging.hpp"
#include "yield/fs/file_system.hpp"

#include <Windows.h>

namespace yield {
namespace fs {
namespace poll {
namespace win32 {
using ::std::shared_ptr;
using ::std::unique_ptr;
using ::std::vector;

DirectoryWatch::DirectoryWatch(
  shared_ptr<Directory> directory,
  FsEvent::Type fs_event_types,
  const Path& path
) : Watch(directory, fs_event_types, path) {
  unique_ptr<Directory::Entry> dirent = directory->read();
  if (dirent != NULL) {
    do {
      if (dirent->ISDIR() && !dirent->is_hidden() && !dirent->is_special()) {
        subdirectory_names.push_back(dirent->name());
      }
    } while (directory->read(*dirent));
    directory->rewind();
  }
}

unique_ptr<FsEvent>
DirectoryWatch::parse(
  const FILE_NOTIFY_INFORMATION& file_notify_info
) {
  log_read(file_notify_info);

  FsEvent::Type fs_event_type;
  Path name(
    file_notify_info.FileName,
    file_notify_info.FileNameLength / sizeof(wchar_t)
  );
  Path path = this->path() / name;

  switch (file_notify_info.Action) {
  case FILE_ACTION_ADDED: {
    if (FileSystem().isdir(path)) {
      subdirectory_names.push_back(name);
      fs_event_type = FsEvent::TYPE_DIRECTORY_ADD;
    } else {
      fs_event_type = FsEvent::TYPE_FILE_ADD;
    }
  }
  break;

  case FILE_ACTION_MODIFIED: {
    if (FileSystem().isdir(path)) {
      fs_event_type = FsEvent::TYPE_DIRECTORY_MODIFY;
    } else {
      fs_event_type = FsEvent::TYPE_FILE_MODIFY;
    }
  }
  break;

  case FILE_ACTION_REMOVED: {
    fs_event_type = FsEvent::TYPE_FILE_REMOVE;

    for (
      auto subdirectory_name_i
      = subdirectory_names.begin();
      subdirectory_name_i != subdirectory_names.end();
      ++subdirectory_name_i
    ) {
      if (*subdirectory_name_i == name) {
        subdirectory_names.erase(subdirectory_name_i);
        fs_event_type = FsEvent::TYPE_DIRECTORY_REMOVE;
        break;
      }
    }
  }
  break;

  case FILE_ACTION_RENAMED_NEW_NAME: {
    CHECK(!old_paths.empty());

    if (FileSystem().isdir(path)) {
      fs_event_type = FsEvent::TYPE_DIRECTORY_RENAME;
      subdirectory_names.push_back(path);
    } else {
      fs_event_type = FsEvent::TYPE_FILE_RENAME;
    }

    if (want_fs_event_type(fs_event_type)) {
      unique_ptr<FsEvent> fs_event(new FsEvent(old_paths.top(), path, fs_event_type));
      log_fs_event(*fs_event);
      old_paths.pop();
      return fs_event;
    } else {
      old_paths.pop();
      return NULL;
    }
  }
  break;

  case FILE_ACTION_RENAMED_OLD_NAME: {
    for (
      auto subdirectory_name_i
      = subdirectory_names.begin();
      subdirectory_name_i != subdirectory_names.end();
      ++subdirectory_name_i
    ) {
      if (*subdirectory_name_i == name) {
        subdirectory_names.erase(subdirectory_name_i);
        break;
      }
    }

    old_paths.push(path);

    return NULL;
  }
  break;

  default:
    CHECK(false);
    return NULL;
  }

  if (want_fs_event_type(fs_event_type)) {
    unique_ptr<FsEvent> fs_event(new FsEvent(path, fs_event_type));
    log_fs_event(*fs_event);
    return fs_event;
  } else {
    return NULL;
  }
}
}
}
}
}
