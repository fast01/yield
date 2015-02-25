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

#include "yield/ssize_t.hpp"
#include "yield/stage/stage.hpp"
#include "yield/stage/wavefront_stage_scheduler.hpp"

namespace yield {
namespace stage {
using ::std::move;
using ::std::unique_ptr;

class WavefrontStageScheduler::StagePoller
    : public PollingStageScheduler::StagePoller {
public:
  StagePoller(unique_ptr<Stage> first_stage)
    : PollingStageScheduler::StagePoller(move(first_stage))
  { }

  // yield::thread::Runnable
  void run() {
    Time visit_timeout(0.5);

    while (should_run()) {
      ::std::vector< ::std::unique_ptr<Stage> >& stages = this->stages();
      size_t stage_i_max = stages.size();

      // Forward
      for (size_t stage_i = 0; stage_i < stage_i_max; stage_i++) {
        if (stages[stage_i]->visit(visit_timeout)) {
          visit_timeout = static_cast<uint64_t>(0);
        } else if (visit_timeout < 1 * Time::NS_IN_S) {
          visit_timeout += 1 * Time::NS_IN_US;
        }
      }

      // Back
      for (ssize_t stage_i = static_cast<ssize_t>(stage_i_max - 1); stage_i >= 0; stage_i--) {
        if (stages[static_cast<size_t>(stage_i)]->visit(visit_timeout)) {
          visit_timeout = static_cast<uint64_t>(0);
        } else if (visit_timeout < 1 * Time::NS_IN_S) {
          visit_timeout += 1 * Time::NS_IN_US;
        }
      }
    }
  }
};


unique_ptr<PollingStageScheduler::StagePoller>
WavefrontStageScheduler::create_stage_poller(
  unique_ptr<Stage> first_stage
) {
  return unique_ptr<StagePoller>(new StagePoller(move(first_stage)));
}
}
}
