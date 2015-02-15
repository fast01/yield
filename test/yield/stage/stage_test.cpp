// stage_test.cpp

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

#include "./test_event.hpp"
#include "./test_event_handler.hpp"
#include "yield/stage/stage_impl.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace stage {
using ::std::unique_ptr;

TEST(Stage, constructor) {
  StageImpl<TestEvent>(unique_ptr<TestEventHandler>(new TestEventHandler));
}

TEST(Stage, handle) {
  StageImpl<TestEvent>(unique_ptr<TestEventHandler>(new TestEventHandler)).handle(unique_ptr<TestEvent>(new TestEvent));
}

TEST(Stage, visit) {
  unique_ptr< StageImpl<TestEvent> > stage(new StageImpl<TestEvent>(unique_ptr<TestEventHandler>(new TestEventHandler)));
  stage->handle(unique_ptr<TestEvent>(new TestEvent));
  bool visit_ret = stage->visit(Time::FOREVER);
  ASSERT_TRUE(visit_ret);
  ASSERT_EQ(static_cast<TestEventHandler&>(stage->event_handler()).get_seen_events_count(), 1);
}
}
}
