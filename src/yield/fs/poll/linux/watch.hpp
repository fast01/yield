// yield/fs/poll/linux/watch.hpp

// Copyright (c) 2014 Minor Gordon
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

#ifndef _YIELD_FS_POLL_LINUX_WATCH_HPP_
#define _YIELD_FS_POLL_LINUX_WATCH_HPP_

#include "../watch.hpp"

#include <map>

struct inotify_event;

namespace yield {
class EventHandler;

namespace fs {
namespace poll {
namespace linux {
class Watch : public ::yield::fs::poll::Watch {
public:
  Watch(
    FsEvent::Type fs_event_types,
    int inotify_fd,
    const Path& path,
    int wd
  );

  ~Watch();

public:
  int get_wd() const {
    return wd;
  }

public:
  YO_NEW_REF FsEvent* parse(const inotify_event&);

public:
  // yield::Object
  const char* get_type_name() const {
    return "yield::fs::poll::linux::Watch";
  }

  // yield::fs::poll::Watch
  bool is_directory_watch() const {
    return true;
  }

private:
  int inotify_fd;
  std::map<uint32_t, Path> old_names;
  int wd;
};

}
}
}
}

#endif
