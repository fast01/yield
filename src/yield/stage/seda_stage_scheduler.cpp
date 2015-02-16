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

#include "yield/logging.hpp"
#include "yield/time.hpp"
#include "yield/stage/seda_stage_scheduler.hpp"
#include "yield/stage/stage.hpp"
#include "yield/thread/runnable.hpp"
#include "yield/thread/thread.hpp"

namespace yield {
namespace stage {
using ::std::make_shared;
using ::std::shared_ptr;
using ::std::unique_ptr;
using ::std::vector;
using ::yield::thread::Runnable;
using ::yield::thread::Thread;

class SedaStageScheduler::SedaStage : public Runnable {
public:
  SedaStage(shared_ptr<Stage> stage)
    : stage_(stage),
      visit_timeout_(5) {
    should_run_ = true;
  }

  void stop() {
    should_run_ = false;
  }

  // yield::thread::Runnable
  void run() {
    while (should_run_) {
      stage_->visit(visit_timeout_);
    }
  }

private:
  bool should_run_;
  shared_ptr<Stage> stage_;
  Time visit_timeout_;
};


SedaStageScheduler::~SedaStageScheduler() {
  for (
    auto thread_i = threads.begin();
    thread_i != threads.end();
    ++thread_i
  ) {
    static_cast<SedaStage&>((*thread_i)->runnable()).stop();
    (*thread_i)->join();
  }
}

void
SedaStageScheduler::schedule(
  unique_ptr<Stage> stage,
  ConcurrencyLevel concurrency_level
) {
  shared_ptr<Stage> stage_shared(stage.release());
  for (int16_t thread_i = 0; thread_i < concurrency_level; thread_i++) {
    threads.push_back(unique_ptr<Thread>(new Thread(unique_ptr<Runnable>(new SedaStage(stage_shared)))));
  }
}
}
}
