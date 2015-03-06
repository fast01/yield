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

#include "yield/fs/stat.hpp"

#include <cstring>

namespace yield {
namespace fs {
Stat::Stat(const struct stat& stbuf)
  : atime_(stbuf.st_atime),
    blksize_(stbuf.st_blksize),
    blocks_(stbuf.st_blocks),
    ctime_(stbuf.st_ctime),
    dev_(stbuf.st_dev),
    gid_(stbuf.st_gid),
    ino_(stbuf.st_ino),
    mode_(stbuf.st_mode),
    mtime_(stbuf.st_mtime),
    nlink_(stbuf.st_nlink),
    rdev_(stbuf.st_rdev),
    size_(stbuf.st_size),
    uid_(stbuf.st_uid) {
}

bool Stat::ISBLK() const {
  return S_ISBLK(mode());
}

bool Stat::ISCHR() const {
  return S_ISCHR(mode());
}

bool Stat::ISDIR() const {
  return S_ISDIR(mode());
}

bool Stat::ISFIFO() const {
  return S_ISFIFO(mode());
}

bool Stat::ISLNK() const {
  return S_ISLNK(mode());
}

bool Stat::ISREG() const {
  return S_ISREG(mode());
}

bool Stat::ISSOCK() const {
  return S_ISSOCK(mode());
}

Stat::operator struct stat() const {
  struct stat stbuf;
  memset(&stbuf, 0, sizeof(stbuf));
  stbuf.st_atime = static_cast<time_t>(atime().as_unix_date_time_s());
  stbuf.st_blksize = blksize();
  stbuf.st_blocks = blocks();
  stbuf.st_ctime = static_cast<time_t>(ctime().as_unix_date_time_s());
  stbuf.st_dev = static_cast<dev_t>(dev());
  stbuf.st_gid = gid();
  stbuf.st_ino = static_cast<ino_t>(ino());
  stbuf.st_mode = mode();
  stbuf.st_mtime = static_cast<time_t>(mtime().as_unix_date_time_s());
  stbuf.st_nlink = nlink();
  stbuf.st_rdev = static_cast<dev_t>(rdev());
  stbuf.st_size = static_cast<off_t>(size());
  stbuf.st_uid = uid();
  return stbuf;
}

//Stat& Stat::operator=(const struct stat& stbuf) {
//  atime = DateTime(stbuf.st_atime);
//  st_blksize = stbuf.st_blksize;
//  st_blocks = stbuf.st_blocks;
//  ctime = DateTime(stbuf.st_ctime);
//  st_dev = stbuf.st_dev;
//  st_gid = stbuf.st_gid;
//  st_ino = stbuf.st_ino;
//  st_mode = stbuf.st_mode;
//  mtime = DateTime(stbuf.st_mtime);
//  st_nlink = stbuf.st_nlink;
//  st_rdev = stbuf.st_rdev;
//  st_size = stbuf.st_size;
//  st_uid = stbuf.st_uid;
//  return *this;
//}

bool Stat::operator==(const Stat& other) const {
  return atime() == other.atime() &&
         blksize() == other.blksize() &&
         blocks() == other.blocks() &&
         ctime() == other.ctime() &&
         dev() == other.dev() &&
         gid() == other.gid() &&
         ino() == other.ino() &&
         mode() == other.mode() &&
         mtime() == other.mtime() &&
         nlink() == other.nlink() &&
         rdev() == other.rdev() &&
         size() == other.size() &&
         uid() == other.uid();
}
}
}
