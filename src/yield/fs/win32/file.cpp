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

#include <memory>

#include "yield/buffers.hpp"
#include "yield/logging.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/stat.hpp"

#include <io.h>
#include <Windows.h>

namespace yield {
namespace fs {
using ::std::string;
using ::std::unique_ptr;
using ::std::vector;

File::Lock::Lock(
  uint64_t start,
  uint64_t len,
  bool exclusive,
  int16_t whence
) : exclusive_(exclusive),
  len_(len),
  start_(start),
  whence_(whence)
{ }

bool File::Lock::is_exclusive() const {
  return exclusive_;
}

uint64_t File::Lock::len() const {
  return len_;
}

uint64_t File::Lock::start() const {
  return start_;
}

int16_t File::Lock::whence() const {
  return whence_;
}


File::Map::Map(
  size_t capacity,
  void* data,
  fd_t fd,
  fd_t file_mapping,
  uint64_t file_offset,
  bool read_only,
  bool shared
) : Buffer(capacity, data, capacity),
  fd_(fd),
  file_mapping_(file_mapping),
  file_offset_(file_offset),
  read_only_(read_only),
  shared_(shared) {
  if (data_ == reinterpret_cast<void*>(-1)) {
    CHECK_EQ(file_mapping, NULL);
  } else {
    CHECK_NOTNULL(data_);
    CHECK_NOTNULL(file_mapping);
    CHECK_NE(file_mapping, INVALID_HANDLE_VALUE);
  }
}

File::Map::~Map() {
  unmap();
  data_ = NULL;
}

bool File::Map::is_read_only() const {
  return read_only_;
}

bool File::Map::is_shared() const {
  return shared_;
}

bool File::Map::sync() {
  return sync(data(), capacity());
}

bool File::Map::sync(size_t offset, size_t length) {
  return sync(static_cast<char*>(data()) + offset, length);
}

bool File::Map::sync(void* ptr, size_t length) {
  if (data() != reinterpret_cast<void*>(-1)) {
    return FlushViewOfFile(ptr, length) == TRUE;
  } else {
    SetLastError(ERROR_INVALID_PARAMETER);
    return false;
  }
}

bool File::Map::unmap() {
  if (file_mapping_ != NULL) {
    if (
      UnmapViewOfFile(data())
      &&
      CloseHandle(file_mapping_)
    ) {
      data_ = reinterpret_cast<void*>(-1);
      file_mapping_ = NULL;
      return true;
    } else {
      CHECK(false);
      return false;
    }
  } else {
    SetLastError(ERROR_INVALID_PARAMETER);
    return false;
  }
}

bool File::datasync() {
  return FlushFileBuffers(*this) != 0;
}

::std::unique_ptr<File> File::dup(fd_t fd) {
  fd_t dup_fd;
  if (
    DuplicateHandle(
      GetCurrentProcess(),
      fd,
      GetCurrentProcess(),
      &dup_fd,
      0,
      FALSE,
      DUPLICATE_SAME_ACCESS
    )
  ) {
    return unique_ptr<File>(new File(unique_fd(dup_fd)));
  } else {
    return unique_ptr<File>();
  }
}

unique_ptr<File> File::dup(FILE* file) {
  return dup(reinterpret_cast<fd_t>(_get_osfhandle(_fileno(file))));
}

unique_ptr<File::Map>
File::mmap(
  size_t length,
  off_t offset,
  bool read_only,
  bool shared
) {
  if (length == SIZE_MAX) {
    ULARGE_INTEGER uliFileSize;
    uliFileSize.LowPart = GetFileSize(*this, &uliFileSize.HighPart);
    if (uliFileSize.LowPart != INVALID_FILE_SIZE) {
      length = static_cast<size_t>(uliFileSize.QuadPart);
    } else {
      return unique_ptr<File::Map>();
    }
  }

  DWORD flags;
  if (shared) {
    flags = read_only ? FILE_MAP_READ : FILE_MAP_WRITE;
  } else {
    flags = FILE_MAP_COPY;
  }
  DWORD prot = read_only ? PAGE_READONLY : PAGE_READWRITE;

  HANDLE hFileMapping;
  LPVOID lpMapAddress;
  if (length > 0) {
    ULARGE_INTEGER uliMaximumSize;
    uliMaximumSize.QuadPart = length;

    hFileMapping =
      CreateFileMapping(
        *this,
        NULL,
        prot,
        uliMaximumSize.HighPart,
        uliMaximumSize.LowPart,
        NULL
      );

    if (hFileMapping != NULL) {   // not INVALID_HANDLE_VALUE
      ULARGE_INTEGER uliFileOffset;
      uliFileOffset.QuadPart = offset;

      lpMapAddress
      = MapViewOfFile(
          hFileMapping,
          flags,
          uliFileOffset.HighPart,
          uliFileOffset.LowPart,
          uliMaximumSize.LowPart
        );

      if (lpMapAddress == NULL) {
        CloseHandle(hFileMapping);
        return unique_ptr<File::Map>();
      }
    } else {
      return unique_ptr<File::Map>();
    }
  } else { // length == 0
    // Can't CreateFileMapping on an empty file; instead
    // return an "empty" mapping (lpMapAddress=NULL).
    hFileMapping = NULL;
    lpMapAddress = reinterpret_cast<LPVOID>(-1);
  }

  return ::std::unique_ptr<Map>(new
         Map(
           length,
           lpMapAddress,
           *this,
           hFileMapping,
           offset,
           read_only,
           shared
         ));
}

ssize_t File::pread(Buffer& buffer, off_t offset) {
  if (!buffer.next_buffer()) {
    ssize_t pread_ret
    = pread(buffer, buffer.capacity() - buffer.size(), offset);
    if (pread_ret > 0) {
      buffer.put(NULL, static_cast<size_t>(pread_ret));
    }
    return pread_ret;
  } else {
    vector<iovec> iov;
    Buffers::as_read_iovecs(buffer, iov);
    ssize_t preadv_ret = preadv(&iov[0], iov.size(), offset);
    if (preadv_ret > 0) {
      Buffers::put(buffer, NULL, preadv_ret);
    }
    return preadv_ret;
  }
}

ssize_t File::pread(void* buf, size_t buflen, off_t offset) {
  OVERLAPPED overlapped;
  ZeroMemory(&overlapped, sizeof(overlapped));

  overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

  if (overlapped.hEvent != INVALID_HANDLE_VALUE) {
    overlapped.Offset = static_cast<DWORD>(offset);
    if (sizeof(off_t) == sizeof(int64_t)) {
      overlapped.OffsetHigh = static_cast<DWORD>(static_cast<uint64_t>(offset) >> 32);
    }

    DWORD dwNumberOfBytesRead;
    if (ReadFile(*this, buf, buflen, &dwNumberOfBytesRead, &overlapped)) {
      CloseHandle(overlapped.hEvent);
      return static_cast<ssize_t>(dwNumberOfBytesRead);
    } else if (GetLastError() == ERROR_IO_PENDING) {
      DWORD dwNumberOfBytesTransferred;
      if (
        GetOverlappedResult(
          *this,
          &overlapped,
          &dwNumberOfBytesTransferred,
          TRUE
        )
      ) {
        CloseHandle(overlapped.hEvent);
        return static_cast<ssize_t>(dwNumberOfBytesTransferred);
      }
    }

    CloseHandle(overlapped.hEvent);
  }

  return -1;
}

ssize_t File::preadv(const iovec* iov, int iovlen, off_t offset) {
  for (int iov_i = 0; iov_i < iovlen; iov_i++) {
    if (!Buffer::is_page_aligned(iov[iov_i])) {
      if (iovlen == 1) {
        return pread(iov[0].iov_base, iov[0].iov_len, offset);
      } else {
        SetLastError(ERROR_INVALID_PARAMETER);
        return -1;
      }
    }
  }

  OVERLAPPED overlapped;
  ZeroMemory(&overlapped, sizeof(overlapped));

  overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

  if (overlapped.hEvent != INVALID_HANDLE_VALUE) {
    overlapped.Offset = static_cast<DWORD>(offset);
    if (sizeof(off_t) == sizeof(int64_t)) {
      overlapped.OffsetHigh = static_cast<DWORD>(static_cast<uint64_t>(offset) >> 32);
    }

    FILE_SEGMENT_ELEMENT* aSegmentArray = new FILE_SEGMENT_ELEMENT[iovlen];
    DWORD dwNumberOfBytesToRead = 0;

    for (int iov_i = 0; iov_i < iovlen; iov_i++) {
      aSegmentArray[iov_i].Buffer = iov[iov_i].iov_base;
      dwNumberOfBytesToRead += iov[iov_i].iov_len;
    }

    if (
      ReadFileScatter(
        *this,
        aSegmentArray,
        dwNumberOfBytesToRead,
        NULL,
        &overlapped
      )
      ||
      GetLastError() == ERROR_IO_PENDING
    ) {
      // You always have to call GetOverlappedResult with ReadFileScatter
      DWORD dwNumberOfBytesTransferred;
      if (
        GetOverlappedResult(
          *this,
          &overlapped,
          &dwNumberOfBytesTransferred,
          TRUE
        )
      ) {
        delete [] aSegmentArray;
        CloseHandle(overlapped.hEvent);
        return static_cast<ssize_t>(dwNumberOfBytesTransferred);
      }
    }

    delete [] aSegmentArray;
  }

  CloseHandle(overlapped.hEvent);

  return -1;
}

ssize_t File::pwrite(const Buffer& buffer, off_t offset) {
  if (!buffer.next_buffer()) {
    return pwrite(buffer, buffer.size(), offset);
  } else {
    vector<iovec> iov;
    Buffers::as_write_iovecs(buffer, iov);
    return pwritev(&iov[0], iov.size(), offset);
  }
}

ssize_t File::pwrite(const void* buf, size_t buflen, off_t offset) {
  OVERLAPPED overlapped;
  ZeroMemory(&overlapped, sizeof(overlapped));

  overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

  if (overlapped.hEvent != INVALID_HANDLE_VALUE) {
    overlapped.Offset = static_cast<DWORD>(offset);
    if (sizeof(off_t) == sizeof(int64_t)) {
      overlapped.OffsetHigh = static_cast<DWORD>(static_cast<uint64_t>(offset) >> 32);
    }

    DWORD dwNumberOfBytesWritten;
    if (WriteFile(*this, buf, buflen, &dwNumberOfBytesWritten, &overlapped)) {
      CloseHandle(overlapped.hEvent);
      return static_cast<ssize_t>(dwNumberOfBytesWritten);
    } else if (GetLastError() == ERROR_IO_PENDING) {
      DWORD dwNumberOfBytesTransferred;
      if (
        GetOverlappedResult(
          *this,
          &overlapped,
          &dwNumberOfBytesTransferred,
          TRUE
        )
      ) {
        CloseHandle(overlapped.hEvent);
        return static_cast<ssize_t>(dwNumberOfBytesTransferred);
      }
    }

    CloseHandle(overlapped.hEvent);
  }

  return -1;
}

ssize_t File::pwritev(const iovec* iov, int iovlen, off_t offset) {
  for (int iov_i = 0; iov_i < iovlen; iov_i++) {
    if (!Buffer::is_page_aligned(iov[iov_i])) {
      if (iovlen == 1) {
        return pwrite(iov[0].iov_base, iov[0].iov_len, offset);
      } else {
        string buffer;
        for (int iov_i = 0; iov_i < iovlen; iov_i++) {
          buffer.append(
            static_cast<const char*>(iov[iov_i].iov_base),
            iov[iov_i].iov_len
          );
        }

        return pwrite(buffer.c_str(), buffer.size(), offset);
      }
    }
  }

  OVERLAPPED overlapped;
  ZeroMemory(&overlapped, sizeof(overlapped));

  overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

  if (overlapped.hEvent != INVALID_HANDLE_VALUE) {
    overlapped.Offset = static_cast<DWORD>(offset);
    if (sizeof(off_t) == sizeof(int64_t)) {
      overlapped.OffsetHigh = static_cast<DWORD>(static_cast<uint64_t>(offset) >> 32);
    }

    FILE_SEGMENT_ELEMENT* aSegmentArray = new FILE_SEGMENT_ELEMENT[iovlen];
    DWORD dwNumberOfBytesToWrite = 0;

    for (int iov_i = 0; iov_i < iovlen; iov_i++) {
      aSegmentArray[iov_i].Buffer = iov[iov_i].iov_base;
      dwNumberOfBytesToWrite += iov[iov_i].iov_len;
    }

    if (
      WriteFileGather(
        *this,
        aSegmentArray,
        dwNumberOfBytesToWrite,
        NULL,
        &overlapped
      )
    ) {
      CloseHandle(overlapped.hEvent);
      delete [] aSegmentArray;
      return static_cast<ssize_t>(dwNumberOfBytesToWrite);
    } else if (GetLastError() == ERROR_IO_PENDING) {
      DWORD dwNumberOfBytesTransferred;
      if (
        GetOverlappedResult(
          *this,
          &overlapped,
          &dwNumberOfBytesTransferred,
          TRUE
        )
      ) {
        CloseHandle(overlapped.hEvent);
        delete [] aSegmentArray;
        return static_cast<ssize_t>(dwNumberOfBytesTransferred);
      }
    }

    delete [] aSegmentArray;
  }

  CloseHandle(overlapped.hEvent);

  return -1;
}

ssize_t File::read(Buffer& buffer) {
  return Channel::read(buffer);
}

ssize_t File::read(void* buf, size_t buflen) {
  return pread(buf, buflen, tell());
}

ssize_t File::readv(const iovec* iov, int iovlen) {
  return preadv(iov, iovlen, tell());
}

off_t File::seek(off_t offset, uint8_t whence) {
  LARGE_INTEGER liOffset;
  liOffset.QuadPart = offset;

  liOffset.LowPart
  = SetFilePointer(*this, liOffset.LowPart, &liOffset.HighPart, whence);

  if (liOffset.LowPart != INVALID_SET_FILE_POINTER) {
    return static_cast<off_t>(liOffset.QuadPart);
  } else {
    return static_cast<off_t>(-1);
  }
}

bool File::setlk(const Lock& lock) {
  return setlk(lock, false);
}

bool File::setlkw(const Lock& lock) {
  return setlk(lock, true);
}

bool File::setlk(const Lock& lock, bool wait) {
  DWORD dwFlags = 0;
  if (lock.is_exclusive()) {
    dwFlags |= LOCKFILE_EXCLUSIVE_LOCK;
  }
  if (!wait) {
    dwFlags |= LOCKFILE_FAIL_IMMEDIATELY;
  }

  OVERLAPPED overlapped;
  ZeroMemory(&overlapped, sizeof(overlapped));

  overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

  if (overlapped.hEvent != INVALID_HANDLE_VALUE) {
    overlapped.Offset = static_cast<DWORD>(lock.start());
    overlapped.OffsetHigh = static_cast<DWORD>(lock.start() >> 32);

    if (
      LockFileEx(
        *this,
        dwFlags,
        0,
        static_cast<DWORD>(lock.len()),
        static_cast<DWORD>(lock.len() >> 32),
        &overlapped
      )
    ) {
      DWORD dwNumberOfBytesTransferred;

      return GetOverlappedResult(
               *this,
               &overlapped,
               &dwNumberOfBytesTransferred,
               TRUE
             )  == TRUE;
    }
  }

  return false;
}

::std::unique_ptr<Stat> File::stat() {
  BY_HANDLE_FILE_INFORMATION by_handle_file_information;
  if (GetFileInformationByHandle(*this, &by_handle_file_information) != 0) {
    return ::std::unique_ptr<Stat>(new Stat(by_handle_file_information));
  } else {
    return unique_ptr<Stat>();
  }
}

bool File::sync() {
  return FlushFileBuffers(*this) != 0;
}

off_t File::tell() {
  ULARGE_INTEGER uliFilePointer;
  LONG lFilePointerHigh = 0;
  uliFilePointer.LowPart
  = SetFilePointer(*this, 0, &lFilePointerHigh, FILE_CURRENT);
  uliFilePointer.HighPart = lFilePointerHigh;
  return static_cast<off_t>(uliFilePointer.QuadPart);
}

bool File::truncate(off_t length) {
  if (seek(length, SEEK_SET) == length) {
    return SetEndOfFile(*this) != 0;
  } else {
    return false;
  }
}

bool File::unlk(const Lock& lock) {
  return UnlockFile(
           *this,
           static_cast<DWORD>(lock.start()),
           static_cast<DWORD>(lock.start() >> 32),
           static_cast<DWORD>(lock.len()),
           static_cast<DWORD>(lock.len() >> 32)
         ) == TRUE;
}

ssize_t File::write(const Buffer& buffer) {
  return Channel::write(buffer);
}

ssize_t File::write(const void* buf, size_t buflen) {
  return pwrite(buf, buflen, tell());
}

ssize_t File::writev(const iovec* iov, int iovlen) {
  return pwritev(iov, iovlen, tell());
}
}
}
