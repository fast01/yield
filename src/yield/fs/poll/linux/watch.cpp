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

#include <iostream>
#include <sys/inotify.h>

namespace yield {
namespace fs {
namespace poll {
namespace linux {
using ::std::map;
using ::std::unique_ptr;

Watch::Watch(
  FsEvent::Type fs_event_types,
  int inotify_fd,
  const Path& path,
  int wd
) : yield::fs::poll::Watch(fs_event_types, path),
  inotify_fd_(inotify_fd),
  wd_(wd) {
}

Watch::~Watch() {
  inotify_rm_watch(inotify_fd_, wd_);
}

unique_ptr<FsEvent> Watch::parse(const inotify_event& inotify_event_) {
  uint32_t mask = inotify_event_.mask;

  bool isdir;
  if ((mask & IN_ISDIR) == IN_ISDIR) {
    mask ^= IN_ISDIR;
    isdir = true;
  } else {
    isdir = false;
  }
  Path name, path;
  if (inotify_event_.len > 1) {
    // len includes a NULL terminator, but may also include
    // one or more bytes of padding. Do a strlen to find the real length.
    name = Path(inotify_event_.name); //, inotify_event_.len - 1);
    path = this->path() / name;
  } else {
    path = this->path();
  }

  DLOG(DEBUG) <<
      "Watch(path=" << this->path() << ", wd=" << wd() << ")"
      << ": read inotify_event(" <<
      "cookie=" << inotify_event_.cookie <<
      ", " <<
      "isdir=" << (isdir ? "true" : "false") <<
      ", " <<
      "len=" << inotify_event_.len <<
      ", " <<
      "mask=" << inotify_event_.mask <<
      ", "
      "name=" << name <<
      ")";

  FsEvent::Type fs_event_type;

  if ((mask & IN_ATTRIB) == IN_ATTRIB) {
    mask ^= IN_ATTRIB;
    fs_event_type =
      isdir ? FsEvent::TYPE_DIRECTORY_MODIFY : FsEvent::TYPE_FILE_MODIFY;
  } else if ((mask & IN_CREATE) == IN_CREATE) {
    mask ^= IN_CREATE;
    fs_event_type =
      isdir ? FsEvent::TYPE_DIRECTORY_ADD : FsEvent::TYPE_FILE_ADD;
  } else if ((mask & IN_DELETE) == IN_DELETE) {
    mask ^= IN_DELETE;
    fs_event_type =
      isdir ? FsEvent::TYPE_DIRECTORY_REMOVE : FsEvent::TYPE_FILE_REMOVE;
  } else if ((mask & IN_DELETE_SELF) == IN_DELETE_SELF) {
    mask ^= IN_DELETE_SELF;
    fs_event_type =
      isdir ? FsEvent::TYPE_DIRECTORY_REMOVE : FsEvent::TYPE_FILE_REMOVE;
  } else if ((mask & IN_IGNORED) == IN_IGNORED) {
    mask ^= IN_IGNORED;
    CHECK(!isdir);
    fs_event_type = FsEvent::TYPE_FILE_REMOVE;
  } else if ((mask & IN_MODIFY) == IN_MODIFY) {
    mask ^= IN_MODIFY;
    fs_event_type =
      isdir ? FsEvent::TYPE_DIRECTORY_MODIFY : FsEvent::TYPE_FILE_MODIFY;
  } else if ((mask & IN_MOVED_FROM) == IN_MOVED_FROM) {
    mask ^= IN_MOVED_FROM;
    CHECK(!name.empty());
    old_names_[inotify_event_.cookie] = name;
    return unique_ptr<FsEvent>();
  } else if ((mask & IN_MOVED_TO) == IN_MOVED_TO) {
    mask ^= IN_MOVED_TO;
    CHECK_EQ(mask, 0);

    fs_event_type =
      isdir ? FsEvent::TYPE_DIRECTORY_RENAME : FsEvent::TYPE_FILE_RENAME;

    auto old_name_i = old_names_.find(inotify_event_.cookie);
    CHECK_NE(old_name_i, old_names_.end());

    if (want_fs_event_type(fs_event_type)) {
      ::std::unique_ptr<FsEvent> fs_event(
        new FsEvent(
          this->path() / old_name_i->second,
          path,
          fs_event_type
        ));
      old_names_.erase(old_name_i);
      log_fs_event(*fs_event);
      return fs_event;
    } else {
      old_names_.erase(old_name_i);
      return unique_ptr<FsEvent>();
    }
  }

  CHECK_EQ(mask, 0);

  if (want_fs_event_type(fs_event_type)) {
    ::std::unique_ptr<FsEvent> fs_event(new FsEvent(path, fs_event_type));
    log_fs_event(*fs_event);
    return fs_event;
  } else {
    return unique_ptr<FsEvent>();
  }
}
}
}
}
}
