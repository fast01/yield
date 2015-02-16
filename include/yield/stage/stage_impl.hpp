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

#ifndef _YIELD_STAGE_STAGE_IMPL_HPP_
#define _YIELD_STAGE_STAGE_IMPL_HPP_

#include "yield/event_handler.hpp"
#include "yield/event_queue.hpp"
#include "yield/time.hpp"
#include "yield/stage/stage.hpp"
#include "yield/stage/synchronized_event_queue.hpp"

namespace yield {
namespace stage {
template <class EventT>
class StageImpl : public EventHandler<EventT>, public Stage {
public:
  StageImpl(::std::unique_ptr< EventHandler<EventT>> event_handler)
    : event_handler_(::std::move(event_handler)),
      event_queue_(new ::yield::stage::SynchronizedEventQueue<EventT>) {
    init();
  }

  StageImpl(::std::unique_ptr< EventHandler<EventT> > event_handler, ::std::unique_ptr< EventQueue<EventT> > event_queue)
    : event_handler_(::std::move(event_handler)),
      event_queue_(::std::move(event_queue)) {
    init();
  }

  virtual ~StageImpl() {
  }

public:
  double arrival_rate_s() const {
    return arrival_rate_s_;
  }

  EventHandler<EventT>& event_handler() {
    return *event_handler_;
  }

  EventQueue<EventT>& event_queue() {
    return *event_queue_;
  }

  double rho() const {
    return rho_;
  }

  double service_rate_s() const {
    return service_rate_s_;
  }

  void visit() {
    ::std::unique_ptr<EventT> event = event_queue_->dequeue();
    if (event == NULL) {
      return;
    }

    event_queue_length_--;

    Time service_time_start(Time::now());

    service(::std::move(event));

    Time service_time(Time::now() - service_time_start);
  }

  bool visit(const Time& timeout) {
    ::std::unique_ptr<EventT> event = event_queue_->timeddequeue(timeout);
    if (event == NULL) {
      return false;
    }

    event_queue_length_--;

    Time service_time_start(Time::now());

    service(::std::move(event));

    Time service_time(Time::now() - service_time_start);

    return true;
  }

public:
  // yield::EventHandler
  void handle(::std::unique_ptr<EventT> event) override {
    tryenqueue(::std::move(event));
  }

protected:
  StageImpl(::std::unique_ptr< EventQueue<EventT> > event_queue)
    : event_queue_(::std::move(event_queue)) {
    init();
  }

  EventQueue<EventT>& get_event_queue() {
    return event_queue_;
  }

private:
  ::std::unique_ptr<EventT> tryenqueue(::std::unique_ptr<EventT> event) {
    event_queue_length_++;
    event_queue_arrival_count_++;

    if (event_queue_->tryenqueue(::std::move(event)) == NULL) {
      return NULL;
    }/* else {
      LOG(ERROR) << "event queue full, stopping.";
      return NULL;
    }*/
    return NULL;
  }

  void init() {
    event_queue_arrival_count_ = 0;
    event_queue_length_ = 0;
  }

  virtual void service(::std::unique_ptr<EventT> event) {
    event_handler_->handle(::std::move(event));
  }

private:
  double arrival_rate_s_;
  ::std::unique_ptr< EventHandler<EventT> > event_handler_;
  ::std::unique_ptr< EventQueue<EventT> > event_queue_;
  uint32_t event_queue_arrival_count_, event_queue_length_;
  double rho_;
  double service_rate_s_;
  // Sampler<uint64_t, 1024, Mutex> service_times;
};
}
}

#endif
