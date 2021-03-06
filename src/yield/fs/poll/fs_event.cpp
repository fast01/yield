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

#include "yield/logging.hpp"
#include "yield/fs/poll/fs_event.hpp"

namespace yield {
namespace fs {
namespace poll {
const FsEvent::Type FsEvent::TYPE_ALL = static_cast<Type>(~0);
const FsEvent::Type FsEvent::TYPE_DIRECTORY_ADD = 1;
const FsEvent::Type FsEvent::TYPE_DIRECTORY_MODIFY = 2;
const FsEvent::Type FsEvent::TYPE_DIRECTORY_REMOVE = 4;
const FsEvent::Type FsEvent::TYPE_DIRECTORY_RENAME = 8;
const FsEvent::Type FsEvent::TYPE_FILE_ADD = 16;
const FsEvent::Type FsEvent::TYPE_FILE_MODIFY = 32;
const FsEvent::Type FsEvent::TYPE_FILE_REMOVE = 64;
const FsEvent::Type FsEvent::TYPE_FILE_RENAME = 128;

FsEvent::FsEvent(const Path& path, Type type)
  : old_path(path), type(type) {
  CHECK(!path.empty());
  CHECK(
    type == TYPE_DIRECTORY_ADD
    ||
    type == TYPE_DIRECTORY_MODIFY
    ||
    type == TYPE_DIRECTORY_REMOVE
    ||
    type == TYPE_FILE_ADD
    ||
    type == TYPE_FILE_MODIFY
    ||
    type == TYPE_FILE_REMOVE
  );
}

FsEvent::FsEvent(const Path& old_path, const Path& new_path, Type type)
  : new_path(new_path), old_path(old_path), type(type) {
  CHECK(!new_path.empty());
  CHECK(!old_path.empty());
  CHECK(type == TYPE_DIRECTORY_RENAME || type == TYPE_FILE_RENAME);
}

std::ostream& operator<<(std::ostream& os, const FsEvent& fs_event) {
  std::string type;
  switch (fs_event.get_type()) {
  case FsEvent::TYPE_DIRECTORY_ADD:
    type = "DIRECTORY_ADD";
    break;
  case FsEvent::TYPE_DIRECTORY_MODIFY:
    type = "DIRECTORY_MODIFY";
    break;
  case FsEvent::TYPE_DIRECTORY_REMOVE:
    type = "DIRECTORY_REMOVE";
    break;
  case FsEvent::TYPE_DIRECTORY_RENAME:
    type = "DIRECTORY_RENAME";
    break;
  case FsEvent::TYPE_FILE_ADD:
    type = "FILE_ADD";
    break;
  case FsEvent::TYPE_FILE_MODIFY:
    type = "FILE_MODIFY";
    break;
  case FsEvent::TYPE_FILE_REMOVE:
    type = "FILE_REMOVE";
    break;
  case FsEvent::TYPE_FILE_RENAME:
    type = "FILE_RENAME";
    break;
  default:
    CHECK(false);
    break;
  }

  switch (fs_event.get_type()) {
  case FsEvent::TYPE_DIRECTORY_RENAME:
  case FsEvent::TYPE_FILE_RENAME: {
    os <<
       "FsEvent("
       "type=" << type <<
       ", " <<
       "old_path=" << fs_event.get_old_path() <<
       ", "
       "new_path=" << fs_event.get_new_path() <<
       ")";
  }
  break;

  default: {
    os <<
       "FsEvent("
       "type=" << type <<
       ", " <<
       "path=" << fs_event.get_path() <<
       ")";
  }
  }

  return os;
}
}
}
}
