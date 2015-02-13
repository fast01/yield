// yield/fs/poll/watches.hpp

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

#ifndef _YIELD_FS_POLL_WATCHES_HPP_
#define _YIELD_FS_POLL_WATCHES_HPP_

#include "yield/logging.hpp"
#include "yield/fs/path.hpp"

#include <map>
#include <utility>

namespace yield {
namespace fs {
namespace poll {
template <class WatchType>
class Watches final {
public:
  typedef typename std::map<Path, ::std::shared_ptr<WatchType> >::const_iterator const_iterator;

public:
  const_iterator begin() const {
    return map_.begin();
  }

  bool empty() const {
    return map_.empty();
  }

  const_iterator end() const {
    return map_.end();
  }

  ::std::shared_ptr<WatchType> erase(const Path& path) {
    auto watch_i = map_.find(path);
    if (watch_i != map_.end()) {
      ::std::shared_ptr<WatchType> watch = ::std::move(watch_i->second);
      map_.erase(watch_i);
      return watch;
    } else {
      return NULL;
    }
  }

  ::std::shared_ptr<WatchType> find(const Path& path) const {
    auto watch_i = map_.find(path);
    if (watch_i != map_.end()) {
      return watch_i->second;
    } else {
      return NULL;
    }
  }

  void insert(const Path& path, ::std::shared_ptr<WatchType> watch) {
    if (map_.find(path) == map_.end()) {
      map_.insert(::std::make_pair(path, watch));
    } else {
      CHECK(false);
    }
  }

private:
  std::map<Path, ::std::shared_ptr<WatchType> > map_;
};
}
}
}

#endif
