// yield/queue/tls_concurrent_queue.hpp

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

#ifndef _YIELD_QUEUE_TLS_CONCURRENT_QUEUE_HPP_
#define _YIELD_QUEUE_TLS_CONCURRENT_QUEUE_HPP_

#include "yield/exception.hpp"
#include "yield/queue/blocking_concurrent_queue.hpp"
#include "yield/thread/thread.hpp"

#include <stack>

namespace yield {
namespace queue {
/**
  A queue that can handle multiple concurrent enqueues and dequeues but may
    block the caller indefinitely in either operation.

  This queue is an optimization of a normal blocking concurrent queue. It minimizes
    contention between threads by storing some queue elements in thread-local
    storage. There are two caveats to using this queue:
    1) Elements may be reordered as an additional cache-related optimization,
      so the queue discipline is not guaranteed to be FIFO.
    2) Callers of trydequeue must continue to poll trydequeue indefinitely in
      order to dequeue elements from thread-local storage.

  Inspired by:
    James R. Larus and Michael Parkes. 2002. Using Cohort-Scheduling to Enhance
      Server Performance. In Proceedings of the General Track of the annual
      conference on USENIX Annual Technical Conference (ATEC '02),
      Carla Schlatter Ellis (Ed.).
      USENIX Association, Berkeley, CA, USA, 103-114.
*/
template <class ElementType>
class TlsConcurrentQueue {
public:
  TlsConcurrentQueue() {
    tls_key_ = ::yield::thread::Thread::self()->key_create();
    if (tls_key_ == static_cast<uintptr_t>(-1)) {
      throw Exception();
    }
  }

  ~TlsConcurrentQueue() {
    ::yield::thread::Thread::self()->key_delete(tls_key_);
    for (auto stack_i = stacks_.begin(); stack_i != stacks_.end(); stack_i++) {
      delete *stack_i;
    }
  }

  /**
    Enqueue a new element.
    @param element the element to enqueue
    @return true if the enqueue was successful.
  */
  bool enqueue(::std::unique_ptr<ElementType> element) {
    ::std::stack< ::std::unique_ptr<ElementType> >* stack = static_cast< ::std::stack< ::std::unique_ptr<ElementType> >*>(::yield::thread::Thread::self()->getspecific(tls_key_));

    if (stack != NULL) {
      stack->push(::std::move(element));
      return true;
    } else {
      return queue_.enqueue(::std::move(element));
    }
  }

  /**
    Try to dequeue an element.
    @return the dequeued element or NULL if the queue was empty
  */
  ::std::unique_ptr<ElementType> trydequeue() {
    ::std::unique_ptr<ElementType> element;

    ::std::stack< ::std::unique_ptr<ElementType> >* stack = static_cast< ::std::stack< ::std::unique_ptr<ElementType> >*>(::yield::thread::Thread::self()->getspecific(tls_key_));

    if (stack != NULL) {
      if (!stack->empty()) {
        element = ::std::move(stack->top());
        stack->pop();
        return element;
      } else {
        return queue_.trydequeue();
      }
    } else {
      stack = new ::std::stack< ::std::unique_ptr<ElementType> >;
      ::yield::thread::Thread::self()->setspecific(tls_key_, stack);
      stacks_.push_back(stack);
      return queue_.trydequeue();
    }
  }

private:
  uintptr_t tls_key_;
  BlockingConcurrentQueue<ElementType> queue_;
  ::std::vector< ::std::stack< ::std::unique_ptr<ElementType> >* > stacks_;
};
}
}

#endif
