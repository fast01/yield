// yield/stage/stage_impl.hpp

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

#ifndef _YIELD_STAGE_STAGE_IMPL_HPP_
#define _YIELD_STAGE_STAGE_IMPL_HPP_

#include "yield/event_handler.hpp"
#include "yield/event_queue.hpp"
#include "yield/time.hpp"
#include "yield/queue/synchronized_event_queue.hpp"
#include "yield/stage/stage.hpp"

namespace yield {
namespace stage {
template <class EventT = Event>
class StageImpl : public EventHandler<EventT>, public Stage {
public:
  Stage(YO_NEW_REF EventHandler<EventT>& event_handler)
    : event_handler(&event_handler),
      event_queue(*new ::yield::queue::SynchronizedEventQueue) {
    init();
  }

  Stage(YO_NEW_REF EventHandler<EventT>& event_handler, YO_NEW_REF EventQueue<EventT>& event_queue)
    : event_handler(&event_handler),
      event_queue(event_queue) {
    init();
  }

  ~Stage() {
    EventQueue<EventT>::dec_ref(event_queue);
    EventHandler<EventT>::dec_ref(event_handler);
  }

public:
  double get_arrival_rate_s() const {
    return arrival_rate_s;
  }

  double get_rho() const {
    return rho;
  }

  double get_service_rate_s() const {
    return service_rate_s;
  }

  void visit() {
    EventT& event = event_queue.dequeue();
    event_queue_length--;

    Time service_time_start(Time::now());

    service(event);

    Time service_time(Time::now() - service_time_start);
  }

  bool visit(const Time& timeout) {
    EventT* event = event_queue.timeddequeue(timeout);

    if (event != NULL) {
      event_queue_length--;

      Time service_time_start(Time::now());

      service(*event);

      Time service_time(Time::now() - service_time_start);

      return true;
    } else {
      return false;
    }
  }

public:
  // yield::Object
  Stage& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::EventHandler
  void handle(YO_NEW_REF Event& event) {
    enqueue(event);
  }

protected:
  Stage(YO_NEW_REF EventQueue<EventT>& event_queue) {
    : event_handler(NULL), event_queue(event_queue) {
    init();
  }

  EventQueue<EventT>& get_event_queue() {
    return event_queue;
  }

private:
  void enqueue(YO_NEW_REF EventT& event) {
    event_queue_length++;
    event_queue_arrival_count++;

    if (event_queue.enqueue(event)) {
      return;
    }/* else {
      LOG(ERROR) << "event queue full, stopping.";
    }*/
  }

  void init() {
    event_queue_arrival_count = 0;
    event_queue_length = 0;
  }

  void service(YO_NEW_REF EventT& event) {
    event_handler->handle(event);
  }

private:
  double arrival_rate_s;
  EventHandler<EventT>* event_handler;
  EventQueue<EventT>& event_queue;
  uint32_t event_queue_arrival_count, event_queue_length;
  double rho;
  double service_rate_s;
  // Sampler<uint64_t, 1024, Mutex> service_times;
};
};
};

#endif
