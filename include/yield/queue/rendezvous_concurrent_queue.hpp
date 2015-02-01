// yield/queue/rendezvous_concurrent_queue.hpp

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

#ifndef _YIELD_QUEUE_RENDEZVOUS_CONCURRENT_QUEUE_HPP_
#define _YIELD_QUEUE_RENDEZVOUS_CONCURRENT_QUEUE_HPP_

#include <yield/types.hpp>

#include <atomic>
#include <memory>

namespace yield {
namespace queue {
/**
  A queue that can store only a single element, aka a rendezvous.
  Queue operations are atomic operations on a pointer to the element.
*/
template <class ElementType>
class RendezvousConcurrentQueue {
public:
  RendezvousConcurrentQueue() {
    element = 0;
  }

  /**
    Enqueue a new element.
    @param element the element to enqueue
    @return true if the enqueue was successful.
  */
  bool enqueue(::std::shared_ptr<ElementType> element) {
    uintptr_t old_element = 0;
    uintptr_t new_element = reinterpret_cast<uintptr_t>(new ::std::shared_ptr<ElementType>(element));
    return this->element.compare_exchange_weak(old_element, new_element);
  }

  /**
    Try to dequeue an element.
    Never blocks.
    @return the dequeued element or NULL if the queue was empty
  */
  ::std::shared_ptr<ElementType> trydequeue() {
    uintptr_t old_element = element.load();
    uintptr_t new_element = 0;
    if (this->element.compare_exchange_weak(old_element, new_element)) {
      if (old_element != NULL) {
        ::std::shared_ptr<ElementType>* old_element_shared_ptr = reinterpret_cast< ::std::shared_ptr<ElementType>* >(old_element);
        ::std::shared_ptr<ElementType> old_element_shared_ptr_copy(*old_element_shared_ptr);
        delete old_element_shared_ptr;
        return old_element_shared_ptr_copy;
      } else {
        return NULL;
      }
    } else {
      return NULL;
    }
  }

private:
  std::atomic_uintptr_t element;
};
}
}

#endif
