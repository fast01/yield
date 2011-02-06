// yield/fs/posix/file_system.hpp

// Copyright (c) 2011 Minor Gordon
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

#ifndef _YIELD_FS_POSIX_FILE_SYSTEM_HPP_
#define _YIELD_FS_POSIX_FILE_SYSTEM_HPP_

#include "yield/object.hpp"

struct statvfs;

namespace yield {
namespace fs {
class Path;

namespace posix {
class ExtendedAttributes;
class File;
class MemoryMappedFile;
class Stat;

class FileSystem {
public:
  static mode_t FILE_MODE_DEFAULT;
  static mode_t DIRECTORY_MODE_DEFAULT;
  static int MMAP_FLAGS_DEFAULT; // MAP_SHARED
  const static size_t MMAP_LENGTH_WHOLE_FILE = static_cast<size_t>(-1);
  static int MMAP_PROT_DEFAULT; // PROT_READ|PROT_WRITE
  static uint32_t OPEN_FLAGS_DEFAULT; // O_RDONLY

public:
  bool access(const Path&, int amode);
  bool chmod(const Path&, mode_t);
  bool chown(const Path&, uid_t);
  bool chown(const Path&, uid_t, gid_t);
  Stat* getattr(const Path&);
  bool link(const Path& old_path, const Path& new_path);
  bool mkdir(const Path&, mode_t mode);

  YO_NEW_REF File*
  mkfifo
  (
    const Path&,
    uint32_t flags = OPEN_FLAGS_DEFAULT,
    mode_t mode = FILE_MODE_DEFAULT
  );

  static void*
  mmap
  (
    void* start,
    size_t length,
    int prot,
    int flags,
    fd_t fd,
    uint64_t offset
  );

  YO_NEW_REF MemoryMappedFile*
  mmap
  (
    File& file,
    void* start = NULL,
    size_t length = MMAP_LENGTH_WHOLE_FILE,
    int prot = MMAP_PROT_DEFAULT,
    int flags = MMAP_FLAGS_DEFAULT,
    uint64_t offset = 0
  );

  virtual YO_NEW_REF File*
  open
  (
    const Path&,
    uint32_t flags = OPEN_FLAGS_DEFAULT,
    mode_t mode = FILE_MODE_DEFAULT,
    uint32_t attributes = OPEN_ATTRIBUTES_DEFAULT
  );

  virtual YO_NEW_REF Directory* opendir(const Path&);
  virtual YO_NEW_REF ExtendedAttributes* openxattrs(const Path&);
  bool readlink(const Path&, OUT Path&);
  bool realpath(const Path&, OUT Path&);
  bool rename(const Path& from_path, const Path& to_path);
  bool rmdir(const Path&);
  bool statvfs(const Path&, struct statvfs&);
  bool symlink(const Path& old_path, const Path& new_path);
  bool truncate(const Path&, uint64_t new_size);
  bool unlink(const Path&);
  bool utime(const Path&, const DateTime& atime, const DateTime& mtime);
};
}
}
}

#endif