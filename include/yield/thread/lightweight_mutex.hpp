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

#ifndef _YIELD_THREAD_LIGHTWEIGHT_MUTEX_HPP_
#define _YIELD_THREAD_LIGHTWEIGHT_MUTEX_HPP_

#ifdef _WIN32
struct _RTL_CRITICAL_SECTION;
typedef struct _RTL_CRITICAL_SECTION RTL_CRITICAL_SECTION;
typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;
#else
#include "yield/thread/mutex.hpp"
#endif

namespace yield {
namespace thread {
#ifdef _WIN32
/**
  Lightweight mutex synchronization primitive.
*/
class LightweightMutex {
public:
  class Holder {
  public:
    Holder(LightweightMutex& lock)
      : lock(lock) {
      lock.lock();
    }

    ~Holder() {
      lock.unlock();
    }

  private:
    LightweightMutex& lock;
  };

public:
  LightweightMutex();
  ~LightweightMutex();

public:
  /**
    Lock the mutex, blocking until acquisition.
    @return true if the caller now holds the mutex
  */
  bool lock();

  /**
    Try to lock the mutex, not blocking on failure.
    @return true if the caller now holds the mutex
  */
  bool trylock();

  /**
    Unlock the mutex.
  */
  void unlock();

private:
  CRITICAL_SECTION* critical_section;
};
#else
typedef Mutex LightweightMutex;
#endif
}
}

#endif
