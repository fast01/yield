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

#include <Windows.h>

namespace yield {
namespace fs {
Stat::Stat(
  const DateTime& atime,
  uint32_t attributes,
  const DateTime& ctime,
  const DateTime& mtime,
  int16_t nlink,
  uint64_t size
) : atime_(atime),
  attributes_(attributes),
  ctime_(ctime),
  mtime_(mtime),
  nlink_(nlink),
  size_(size)
{ }

Stat::Stat(const BY_HANDLE_FILE_INFORMATION& stbuf)
  : atime_(stbuf.ftLastAccessTime),
    attributes_(stbuf.dwFileAttributes),
    ctime_(stbuf.ftCreationTime),
    mtime_(stbuf.ftLastWriteTime),
    nlink_(static_cast<int16_t>(stbuf.nNumberOfLinks)) {
  set_size(stbuf.nFileSizeLow, stbuf.nFileSizeHigh);
}

Stat::Stat(const WIN32_FILE_ATTRIBUTE_DATA& stbuf)
  : atime_(stbuf.ftLastAccessTime),
    attributes_(stbuf.dwFileAttributes),
    ctime_(stbuf.ftCreationTime),
    mtime_(stbuf.ftLastWriteTime),
    nlink_(1) {
  set_size(stbuf.nFileSizeLow, stbuf.nFileSizeHigh);
}

Stat::Stat(const WIN32_FIND_DATA& stbuf)
  : atime_(stbuf.ftLastAccessTime),
    attributes_(stbuf.dwFileAttributes),
    ctime_(stbuf.ftCreationTime),
    mtime_(stbuf.ftLastWriteTime),
    nlink_(1) {
  set_size(stbuf.nFileSizeLow, stbuf.nFileSizeHigh);
}

bool Stat::ISDEV() const {
  return (attributes_ & FILE_ATTRIBUTE_DEVICE) == FILE_ATTRIBUTE_DEVICE;
}

bool Stat::ISDIR() const {
  return (attributes_ & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
}

bool Stat::ISREG() const {
  return !ISDEV() && !ISDIR();
}

bool Stat::operator==(const Stat& other) const {
  return atime() == other.atime() &&
         attributes() == other.attributes() &&
         ctime() == other.ctime() &&
         mtime() == other.mtime() &&
         nlink() == other.nlink() &&
         size() == other.size();
}

Stat::operator BY_HANDLE_FILE_INFORMATION() const {
  BY_HANDLE_FILE_INFORMATION bhfi;
  memset(&bhfi, 0, sizeof(bhfi));
  bhfi.dwFileAttributes = attributes();
  bhfi.ftCreationTime = ctime();
  bhfi.ftLastAccessTime = atime();
  bhfi.ftLastWriteTime = mtime();
  ULARGE_INTEGER size;
  size.QuadPart = this->size();
  bhfi.nFileSizeLow = size.LowPart;
  bhfi.nFileSizeHigh = size.HighPart;
  bhfi.nNumberOfLinks = nlink();
  return bhfi;
}

Stat::operator WIN32_FILE_ATTRIBUTE_DATA() const {
  WIN32_FILE_ATTRIBUTE_DATA file_attribute_data;
  memset(&file_attribute_data, 0, sizeof(file_attribute_data));
  file_attribute_data.dwFileAttributes = attributes();
  file_attribute_data.ftCreationTime = ctime();
  file_attribute_data.ftLastAccessTime = atime();
  file_attribute_data.ftLastWriteTime = mtime();
  ULARGE_INTEGER size;
  size.QuadPart = this->size();
  file_attribute_data.nFileSizeLow = size.LowPart;
  file_attribute_data.nFileSizeHigh = size.HighPart;
  return file_attribute_data;
}

Stat::operator WIN32_FIND_DATA() const {
  WIN32_FIND_DATA find_data;
  memset(&find_data, 0, sizeof(find_data));
  find_data.dwFileAttributes = attributes();
  find_data.ftCreationTime = ctime();
  find_data.ftLastAccessTime = atime();
  find_data.ftLastWriteTime = mtime();
  ULARGE_INTEGER size;
  size.QuadPart = this->size();
  find_data.nFileSizeLow = size.LowPart;
  find_data.nFileSizeHigh = size.HighPart;
  return find_data;
}

Stat& Stat::operator=(const BY_HANDLE_FILE_INFORMATION& stbuf) {
  atime_ = stbuf.ftLastAccessTime;
  attributes_ = stbuf.dwFileAttributes;
  ctime_ = stbuf.ftCreationTime;
  mtime_ = stbuf.ftLastWriteTime;
  nlink_ = static_cast<int16_t>(stbuf.nNumberOfLinks);
  set_size(stbuf.nFileSizeLow, stbuf.nFileSizeHigh);
  return *this;
}

Stat& Stat::operator=(const WIN32_FILE_ATTRIBUTE_DATA& stbuf) {
  atime_ = stbuf.ftLastAccessTime;
  attributes_ = stbuf.dwFileAttributes;
  ctime_ = stbuf.ftCreationTime;
  mtime_ = stbuf.ftLastWriteTime;
  nlink_ = 1; // WIN32_FILE_ATTRIBUTE_DATA doesn't have a nNumberOfLinks
  set_size(stbuf.nFileSizeLow, stbuf.nFileSizeHigh);
  return *this;
}

Stat& Stat::operator=(const WIN32_FIND_DATA& stbuf) {
  atime_ = stbuf.ftLastAccessTime;
  attributes_ = stbuf.dwFileAttributes;
  ctime_ = stbuf.ftCreationTime;
  mtime_ = stbuf.ftLastWriteTime;
  nlink_ = 1; // WIN32_FIND_DATA doesn't have a nNumberOfLinks
  set_size(stbuf.nFileSizeLow, stbuf.nFileSizeHigh);
  return *this;
}

void Stat::set_size(uint32_t nFileSizeLow, uint32_t nFileSizeHigh) {
  ULARGE_INTEGER uliSize;
  uliSize.LowPart = nFileSizeLow;
  uliSize.HighPart = nFileSizeHigh;
  size_ = static_cast<size_t>(uliSize.QuadPart);
}
}
}
