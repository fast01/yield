// yield/fs/aio/posix/aio_queue.cpp

// Copyright (c) 2011 Minor Gordon
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

#include "aio_queue.hpp"
#include "yield/fs/aio/fsync_aiocb.hpp"
#include "yield/fs/aio/pread_aiocb.hpp"
#include "yield/fs/aio/pwrite_aiocb.hpp"
#include "yield/stage/synchronized_event_queue.hpp"


namespace yield {
namespace aio {
namespace fs {
namespace posix {
using yield::fs::aio::AIOCB;
using yield::stage::SynchronizedEventQueue;


AIOQueue::AIOQueue() {
  completed_event_queue = new SynchronizedEventQueue;
}

AIOQueue::~AIOQueue() {
  SynchronizedEventQueue::dec_ref(*completed_event_queue);
}

YO_NEW_REF Event& AIOQueue::dequeue() {
  return completed_event_queue->dequeue();
}

YO_NEW_REF Event* AIOQueue::dequeue(const Time& timeout) {
  return completed_event_queue->dequeue(timeout);
}

bool AIOQueue::enqueue(YO_NEW_REF AIOCB& aiocb) {
  return aiocb.issue(*completed_event_queue);
}

bool AIOQueue::enqueue(YO_NEW_REF Event& event) {
  switch (event.get_type_id()) {
  case fsyncAIOCB::TYPE_ID:
  case preadAIOCB::TYPE_ID:
  case pwriteAIOCB::TYPE_ID:
    return enqueue(static_cast<AIOCB&>(event));

  default:
    return completed_event_queue->enqueue(event);
  }
}

YO_NEW_REF Event* AIOQueue::trydequeue() {
  return completed_event_queue->trydequeue();
}
}
}
}
}