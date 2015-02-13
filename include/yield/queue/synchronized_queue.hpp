// yield/queue/synchronized_queue.hpp

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

#ifndef _YIELD_QUEUE_SYNCHRONIZED_QUEUE_HPP_
#define _YIELD_QUEUE_SYNCHRONIZED_QUEUE_HPP_

#include "yield/time.hpp"
#include "yield/thread/condition_variable.hpp"

#include <queue>

namespace yield {
namespace queue {
/**
  A queue whose operations are synchronized by a condition variable.

  The queue can handle multiple concurrent enqueues and dequeues but may block the caller
    indefinitely in either operation.
*/
template <class ElementT>
class SynchronizedQueue {
public:
  /**
    Dequeue an element.
    Always succeeds.
    @return the dequeued element
  */
  ::std::unique_ptr<ElementT> dequeue() {
    cond_.lock_mutex();

    while (queue_.empty()) {
      cond_.wait();
    }

    ::std::unique_ptr<ElementT> element(::std::move(queue_.front()));
    queue_.pop();

    cond_.unlock_mutex();

    return element;
  }

  /**
    Dequeue an element, blocking until a timeout if the queue is empty.
    @param timeout time to block on an empty queue
    @return the dequeued element or NULL if queue was empty for the duration
      of the timeout
  */
  ::std::unique_ptr<ElementT> timeddequeue(const Time& timeout) {
    Time timeout_left(timeout);

    cond_.lock_mutex();

    if (!queue_.empty()) {
      ::std::unique_ptr<ElementT> element(::std::move(queue_.front()));
      queue_.pop();
      cond_.unlock_mutex();
      return element;
    } else {
      for (;;) {
        Time start_time = Time::now();

        cond_.timedwait(timeout_left);

        if (!queue_.empty()) {
          ::std::unique_ptr<ElementT> element(::std::move(queue_.front()));
          queue_.pop();
          cond_.unlock_mutex();
          return element;
        } else {
          Time elapsed_time(Time::now() - start_time);
          if (elapsed_time < timeout_left) {
            timeout_left -= elapsed_time;
          } else {
            cond_.unlock_mutex();
            return NULL;
          }
        }
      }
    }
  }

  /**
    Try to dequeue an element.
    Never blocks.
    @return the dequeue element or NULL if the queue was empty
  */
  ::std::unique_ptr<ElementT> trydequeue() {
    if (cond_.trylock_mutex()) {
      if (!queue_.empty()) {
        ::std::unique_ptr<ElementT> element(::std::move(queue_.front()));
        queue_.pop();
        cond_.unlock_mutex();
        return element;
      } else {
        cond_.unlock_mutex();
      }
    }

    return NULL;
  }

  /**
    Enqueue a new element.
    @param element the element to enqueue
    @return NULL if the enqueue was successful, otherwise the element
  */
  ::std::unique_ptr<ElementT> tryenqueue(::std::unique_ptr<ElementT> element) {
    cond_.lock_mutex();
    queue_.push(::std::move(element));
    cond_.signal();
    cond_.unlock_mutex();
    return NULL;
  }

  /**
    Interrupt a blocking dequeue.
   */
  void wake() {
    tryenqueue(::std::unique_ptr<ElementT>());
  }

private:
  yield::thread::ConditionVariable cond_;
  std::queue< ::std::unique_ptr<ElementT> > queue_;
};
}
}

#endif
