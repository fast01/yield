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

#include "./directory_watch.hpp"
#include "./file_watch.hpp"
#include "yield/exception.hpp"
#include "yield/logging.hpp"
#include "yield/fs/poll/fs_event_queue.hpp"
#include "yield/fs/file_system.hpp"

#include <Windows.h>

namespace yield {
namespace fs {
namespace poll {
using ::std::make_pair;
using ::std::move;
using ::std::shared_ptr;
using ::std::unique_ptr;
using win32::DirectoryWatch;
using win32::FileWatch;

FsEventQueue::FsEventQueue() {
  hIoCompletionPort_.reset(CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0));
  if (!hIoCompletionPort_) {
    throw Exception();
  }
}

FsEventQueue::~FsEventQueue() {
}

bool FsEventQueue::associate(const Path& path, FsEvent::Type fs_event_types) {
  auto watch_i = watches_.find(path);
  if (watch_i != watches_.end()) {
    if (watch_i->second->fs_event_types() == fs_event_types) {
      return true;
    } else {
      watches_.erase(watch_i);
    }
  }

  Path directory_path;
  if (FileSystem().isdir(path)) {
    directory_path = path;
  } else {
    directory_path = path.split().first;
  }

  shared_ptr<Directory> directory = FileSystem().opendir(directory_path);
  if (directory != NULL) {
    if (
      CreateIoCompletionPort(
        *directory,
        *hIoCompletionPort_,
        0,
        0
      ) != INVALID_HANDLE_VALUE
    ) {
      unique_ptr<win32::Watch> watch;
      if (path == directory_path) {
        watch.reset(new DirectoryWatch(directory, fs_event_types, path));
      } else {
        watch.reset(new FileWatch(
          directory,
          directory_path,
          path,
          fs_event_types
        ));
      }

      DWORD dwBytesRead = 0;
      if (
        ReadDirectoryChangesW(
          *directory,
          watch->buffer(),
          watch->buffer_length(),
          FALSE,
          watch->notify_filter(),
          &dwBytesRead,
          *watch,
          NULL
        )
      ) {
        CHECK_EQ(dwBytesRead, 0);
        CHECK(watches_.find(path) == watches_.end());
        watches_.insert(make_pair(path, move(watch)));
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool FsEventQueue::dissociate(const Path& path) {
  auto watch_i = watches_.find(path);
  if (watch_i != watches_.end()) {
    watch_i->second->close();
    watches_.erase(watch_i);
    return true;
  } else {
    return false;
  }
}

unique_ptr<FsEvent> FsEventQueue::tryenqueue(unique_ptr<FsEvent> event) {
  CHECK(false);
  return unique_ptr<FsEvent>();
  //return PostQueuedCompletionStatus(
  //         hIoCompletionPort,
  //         0,
  //         reinterpret_cast<ULONG_PTR>(&event),
  //         NULL
  //       )
  //       == TRUE;
}

unique_ptr<FsEvent> FsEventQueue::timeddequeue(const Time& timeout) {
  unique_ptr<FsEvent> event = event_queue_.trydequeue();
  if (event != NULL) {
    return event;
  }

  DWORD dwBytesTransferred = 0;
  ULONG_PTR ulCompletionKey = 0;
  LPOVERLAPPED lpOverlapped = NULL;

  BOOL bRet
  = GetQueuedCompletionStatus(
      *hIoCompletionPort_,
      &dwBytesTransferred,
      &ulCompletionKey,
      &lpOverlapped,
      static_cast<DWORD>(timeout.ms())
    );

  if (lpOverlapped != NULL) {
    win32::Watch& watch = win32::Watch::cast(*lpOverlapped);
    if (watch.is_closed()) {
      return unique_ptr<FsEvent>();
    }
    CHECK_EQ(bRet, TRUE);
    CHECK_GT(dwBytesTransferred, 0);

    DWORD dwReadUntilBufferOffset = 0;
    for (;;) {
      const FILE_NOTIFY_INFORMATION* file_notify_info
      = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(
          &watch.buffer()[dwReadUntilBufferOffset]
        );

      unique_ptr<FsEvent> fs_event = watch.parse(*file_notify_info);
      if (fs_event != NULL) {
        event_queue_.tryenqueue(move(fs_event));
      }

      if (file_notify_info->NextEntryOffset > 0) {
        dwReadUntilBufferOffset += file_notify_info->NextEntryOffset;
      } else {
        break;
      }
    }

    DWORD dwBytesRead;
    //BOOL bRet =
    ReadDirectoryChangesW(
      watch.directory(),
      watch.buffer(),
      watch.buffer_length(),
      FALSE,
      watch.notify_filter(),
      &dwBytesRead,
      watch,
      NULL
    );

    return trydequeue();
  //} else if (ulCompletionKey != 0) {
  //  return reinterpret_cast<FsEvent*>(ulCompletionKey);
  }

  return unique_ptr<FsEvent>();
}
}
}
}
