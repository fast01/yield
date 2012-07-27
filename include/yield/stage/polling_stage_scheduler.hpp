// yield/stage/polling_stage_scheduler.hpp

// Copyright (c) 2012 Minor Gordon
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

#ifndef _YIELD_STAGE_POLLING_STAGE_SCHEDULER_HPP_
#define _YIELD_STAGE_POLLING_STAGE_SCHEDULER_HPP_

#include "yield/queue/rendezvous_concurrent_queue.hpp"
#include "yield/stage/stage_scheduler.hpp"
#include "yield/thread/runnable.hpp"
#include "yield/thread/thread.hpp"

namespace yield {
namespace stage {
class PollingStageScheduler : public StageScheduler {
public:
  virtual ~PollingStageScheduler();

  // StageScheduler
  void schedule(Stage&, ConcurrencyLevel);

protected:
  class StagePoller : public ::yield::thread::Runnable {
  public:
    virtual ~StagePoller();

    void schedule(Stage&);
    void stop() {
      _should_run = false;
    }

  protected:
    StagePoller(Stage&);

    vector<Stage*>& get_stages();
    inline bool should_run() const {
      return _should_run;
    }

  private:
    ::yield::queue::RendezvousConcurrentQueue<Stage> new_stage;
    bool _should_run;
    vector<Stage*> stages;
  };

protected:
  PollingStageScheduler() { }

  virtual YO_NEW_REF StagePoller& createStagePoller(Stage&) = 0;

private:
  vector< ::yield::thread::Thread*> threads;
};
}
}

#endif
