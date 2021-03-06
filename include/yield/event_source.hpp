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

#ifndef _YIELD_EVENT_SOURCE_HPP_
#define _YIELD_EVENT_SOURCE_HPP_

#include <memory>

#include "yield/time.hpp"

namespace yield {
/**
  Interface for event sources in the event-driven concurrency subsystem.
*/
template <class EventT>
class EventSource {
public:
  /**
    Empty virtual destructor.
  */
  virtual ~EventSource() { }

  /**
    Dequeue an event, blocking indefinitely until one becomes available or wake() is called.
    @return an event if one is available, NULL if wake() was called before that
  */
  virtual ::std::unique_ptr<EventT> dequeue() = 0;

  /**
    Dequeue an event, blocking until one becomes available, the timeout expires, or wake() is called.
    @param timeout the time to wait for new events
    @return an event if one is available within the timeout, otherwise NULL
  */
  virtual ::std::unique_ptr<EventT> timeddequeue(const Time& timeout) = 0;

  /**
    Dequeue an event, non-blocking.
    @return an event if one is available immediately, otherwise NULL
  */
  virtual ::std::unique_ptr<EventT> trydequeue() = 0;

  /**
    Interrupt a blocking dequeue.
   */
  virtual void wake() = 0;
};
}

#endif
