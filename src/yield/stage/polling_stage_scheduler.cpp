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

#include "yield/stage/polling_stage_scheduler.hpp"
#include "yield/stage/stage.hpp"

namespace yield {
namespace stage {
using ::std::move;
using ::std::unique_ptr;
using ::std::vector;
using ::yield::thread::Thread;

PollingStageScheduler::~PollingStageScheduler() {
  for (
    auto thread_i = threads.begin();
    thread_i != threads.end();
    ++thread_i
  ) {
    static_cast<StagePoller&>((*thread_i)->runnable()).stop();
    (*thread_i)->join();
  }
}

void
PollingStageScheduler::schedule(
  unique_ptr<Stage> stage,
  ConcurrencyLevel concurrency_level
) {
  for (uint16_t thread_i = 0; thread_i < concurrency_level; thread_i++) {
    if (thread_i < threads.size()) {
      Thread* thread = threads[thread_i % threads.size()].get();
      StagePoller& stage_poller
      = static_cast<StagePoller&>(thread->runnable());
      stage_poller.schedule(move(stage));
    } else {
      threads.push_back(unique_ptr<Thread>(new Thread(move(create_stage_poller(move(stage))))));
    }
  }
}


PollingStageScheduler::StagePoller::StagePoller(unique_ptr<Stage> first_stage) {
  stages_.push_back(::std::move(first_stage));
}

vector< unique_ptr<Stage> >& PollingStageScheduler::StagePoller::stages() {
  unique_ptr<Stage> new_stage = this->new_stage_.trydequeue();
  while (new_stage != NULL) {
    stages_.push_back(move(new_stage));
    new_stage = this->new_stage_.trydequeue();
  }
  return stages_;
}

void PollingStageScheduler::StagePoller::schedule(unique_ptr<Stage> stage) {
  while (stage != NULL) {
    stage = new_stage_.tryenqueue(move(stage));
  }
}
}
}
