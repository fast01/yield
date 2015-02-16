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

#ifndef _YIELD_QUEUE_BLOCKING_CONCURRENT_QUEUE_HPP_
#define _YIELD_QUEUE_BLOCKING_CONCURRENT_QUEUE_HPP_

#include "yield/queue/concurrent_queue.hpp"
#include "yield/thread/mutex.hpp"

#include <queue>

namespace yield {
namespace queue {
/**
  A queue that can handle multiple concurrent enqueues and dequeues but may
    block the caller indefinitely in either operation.
*/
template <class ElementT>
class BlockingConcurrentQueue final : public ConcurrentQueue<ElementT> {
public:
  ::std::unique_ptr<ElementT> trydequeue() override {
    ::yield::thread::Mutex::Holder mutex_holder(mutex_);
    if (!queue_.empty()) {
      ::std::unique_ptr<ElementT> element(::std::move(queue_.front()));
      queue_.pop();
      return element;
    } else {
      return NULL;
    }
  }

  ::std::unique_ptr<ElementT> tryenqueue(::std::unique_ptr<ElementT> element) override {
    ::yield::thread::Mutex::Holder mutex_holder(mutex_);
    queue_.push(::std::move(element));
    return NULL;
  }

private:
  ::yield::thread::Mutex mutex_;
  ::std::queue< ::std::unique_ptr<ElementT> > queue_;
};
}
}

#endif
