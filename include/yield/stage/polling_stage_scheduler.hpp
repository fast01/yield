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

#ifndef _YIELD_STAGE_POLLING_STAGE_SCHEDULER_HPP_
#define _YIELD_STAGE_POLLING_STAGE_SCHEDULER_HPP_

#include <vector>

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
  void schedule(::std::unique_ptr<Stage>, ConcurrencyLevel) override;

protected:
  class StagePoller : public ::yield::thread::Runnable {
  public:
    virtual ~StagePoller() {
    }

  public:
    void schedule(::std::unique_ptr<Stage>);

    void stop() {
      should_run_ = false;
    }

  protected:
    StagePoller(::std::unique_ptr<Stage>);

    ::std::vector< ::std::unique_ptr<Stage> >& stages();

    inline bool should_run() const {
      return should_run_;
    }

  private:
    ::yield::queue::RendezvousConcurrentQueue<Stage> new_stage_;
    bool should_run_;
    ::std::vector< ::std::unique_ptr<Stage> > stages_;
  };

protected:
  PollingStageScheduler() { }

  virtual ::std::unique_ptr<StagePoller> create_stage_poller( ::std::unique_ptr<Stage> ) = 0;

private:
  ::std::vector< ::std::unique_ptr< ::yield::thread::Thread > > threads;
};
}
}

#endif
