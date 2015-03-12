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

#ifndef _YIELD_QUEUE_NON_BLOCKING_CONCURRENT_QUEUE_HPP_
#define _YIELD_QUEUE_NON_BLOCKING_CONCURRENT_QUEUE_HPP_

#include <atomic>

#include "yield/queue/concurrent_queue.hpp"

namespace yield {
namespace queue {
/**
  A finite queue that can handle multiple concurrent enqueues and dequeues
    without ever blocking the caller. Also called a "lock-free" queue.

  Adapted from Michael, M. M. and Scott, M. L. 1996. Simple, fast, and practical
    non-blocking and blocking concurrent queue algorithms.
*/
template <class ElementT, size_t Length>
class NonBlockingConcurrentQueue : public ConcurrentQueue<ElementT> {
public:
  NonBlockingConcurrentQueue() {
    elements[0] = 1; // Sentinel element
    for (size_t element_i = 1; element_i < Length + 2; element_i++) {
      elements[element_i] = 0;
    }

    head_element_i = 0;
    tail_element_i = 1;
  }

  ::std::unique_ptr<ElementT> trydequeue() override {
    for (;;) {
      size_t head_element_i_copy = head_element_i.load();
      size_t try_element_i = (head_element_i_copy + 1) % (Length + 2);
      uintptr_t try_element = elements[try_element_i].load();

      while (try_element == 0 || try_element == 1) {
        if (head_element_i_copy != head_element_i) {
          break;
        }
        if (try_element_i == tail_element_i) {
          return 0;
        }
        try_element_i = (try_element_i + 1) % (Length + 2);
        try_element = elements[try_element_i];
      }

      if (head_element_i_copy != head_element_i) {
        continue;
      }

      if (try_element_i == tail_element_i) {
        size_t old_tail_element_i = try_element_i;
        size_t new_tail_element_i = (try_element_i + 1) % (Length + 2);
        tail_element_i.compare_exchange_weak(old_tail_element_i, new_tail_element_i);

        continue;
      }

      if (head_element_i_copy != head_element_i) {
        continue;
      }

      uintptr_t new_element_uintptr = static_cast<uintptr_t>((try_element & POINTER_HIGH_BIT) ? 1 : 0);
      uintptr_t old_element = try_element;
      if (
        elements[try_element_i].compare_exchange_weak(
          old_element,
          new_element_uintptr
        )
      ) {
        if (try_element_i % 2 == 0) {
          size_t new_head_element_i = try_element_i;
          size_t old_head_element_i = head_element_i_copy;
          head_element_i.compare_exchange_weak(old_head_element_i, new_head_element_i);
        }

        uintptr_t return_element = try_element;
        return_element &= POINTER_LOW_BITS;
        return_element <<= 1;
        return ::std::unique_ptr<ElementT>(reinterpret_cast<ElementT*>(return_element));
      }
    }
  }

  ::std::unique_ptr<ElementT> tryenqueue(::std::unique_ptr<ElementT> element) override {
    ElementT* new_element = element.release();
    uintptr_t new_element_uintptr = reinterpret_cast<uintptr_t>(new_element);
    // CHECK(!(new_element_uintptr & 1));
    new_element_uintptr >>= 1;
    // CHECK(!(new_element_uintptr & POINTER_HIGH_BIT));

    for (;;) {
      size_t tail_element_i_copy = tail_element_i.load(); // te
      size_t last_try_element_i = tail_element_i_copy; // ate
      uintptr_t try_element = elements[last_try_element_i].load();
      size_t try_element_i = (last_try_element_i + 1) % (Length + 2); // temp

      while (try_element != 0 && try_element != 1) {
        if (tail_element_i_copy != tail_element_i) {
          break;
        }
        if (try_element_i == head_element_i) {
          break;
        }
        try_element = elements[try_element_i];
        last_try_element_i = try_element_i;
        try_element_i = (try_element_i + 1) % (Length + 2);
      }

      if (tail_element_i_copy != tail_element_i) {
        continue;
      }

      if (try_element_i == head_element_i) {
        last_try_element_i = try_element_i;
        try_element_i = (try_element_i + 1) % (Length + 2);
        try_element = elements[try_element_i];

        if (try_element != 0 && try_element != 1) {
          // Queue is full.
          // The element parameter above was .released() into new_element_uintptr.
          return ::std::unique_ptr<ElementT>(reinterpret_cast<ElementT*>(new_element));
        }

        size_t old_head_element_i = last_try_element_i;
        size_t new_head_element_i = try_element_i;
        head_element_i.compare_exchange_weak(old_head_element_i, new_head_element_i);

        continue;
      }

      if (tail_element_i_copy != tail_element_i) {
        continue;
      }

      uintptr_t old_element = try_element;
      if (
        elements[last_try_element_i].compare_exchange_weak(
          old_element,
          try_element == 1
            ? (new_element_uintptr | POINTER_HIGH_BIT)
            : new_element_uintptr
          )
      ) {
        if (try_element_i % 2 == 0) {
          size_t old_tail_element_i = tail_element_i_copy;
          size_t new_tail_element_i = try_element_i;
          tail_element_i.compare_exchange_weak(old_tail_element_i, new_tail_element_i);
        }

        return ::std::unique_ptr<ElementT>();
      }
    }
  }

private:
  const static uintptr_t POINTER_HIGH_BIT
    = static_cast<intptr_t>(1) << ((sizeof(uintptr_t) * 8) - 1);
  const static uintptr_t POINTER_LOW_BITS = ~POINTER_HIGH_BIT;

private:
  ::std::atomic_uintptr_t elements[Length + 2];
  ::std::atomic_size_t head_element_i, tail_element_i;
};
}
}

#endif
