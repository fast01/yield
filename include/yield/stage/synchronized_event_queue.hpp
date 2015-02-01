// yield/stage/synchronized_event_queue.hpp

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

#ifndef _YIELD_STAGE_SYNCHRONIZED_EVENT_QUEUE_HPP_
#define _YIELD_STAGE_SYNCHRONIZED_EVENT_QUEUE_HPP_

#include "yield/event_queue.hpp"
#include "yield/queue/synchronized_queue.hpp"

namespace yield {
namespace stage {
/**
  An EventQueue implementation that wraps a SynchronizedQueue.
*/
template <class EventT>
class SynchronizedEventQueue : public EventQueue<EventT> {
public:
  // yield::EventQueue
  ::std::shared_ptr<EventT> dequeue() {
    return synchronized_queue_.dequeue();
  }

  bool enqueue(::std::shared_ptr<EventT> event) {
    return synchronized_queue_.enqueue(event);
  }

  ::std::shared_ptr<EventT> timeddequeue(const Time& timeout) {
    return synchronized_queue_.timeddequeue(timeout);
  }

  ::std::shared_ptr<EventT> trydequeue() {
    return synchronized_queue_.trydequeue();
  }

  void wake() {
    return synchronized_queue_.wake();
  }

private:
 ::yield::queueSynchronizedQueue<EventT> synchronized_queue_;
};
}
}

#endif
