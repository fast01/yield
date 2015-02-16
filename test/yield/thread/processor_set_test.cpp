// processor_set_test.cpp

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

#include "yield/thread/processor_set.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace thread {
TEST(ProcessorSet, clear_all) {
  ProcessorSet processor_set;
  processor_set.set(0);
  processor_set.clear();
  ASSERT_EQ(processor_set.count(), 0);
}

TEST(ProcessorSet, clear_one) {
  ProcessorSet processor_set;
  processor_set.set(0);
  processor_set.set(1);
  processor_set.clear(0);
  ASSERT_EQ(processor_set.count(), 1);
}

TEST(ProcessorSet, count) {
  ProcessorSet processor_set;
  processor_set.set(0);
  ASSERT_EQ(processor_set.count(), 1);
  processor_set.set(1);
  ASSERT_EQ(processor_set.count(), 2);
  processor_set.clear();
  ASSERT_EQ(processor_set.count(), 0);
}

TEST(ProcessorSet, empty) {
  ProcessorSet processor_set;
  ASSERT_TRUE(processor_set.empty());
  processor_set.set(0);
  ASSERT_FALSE(processor_set.empty());
}

TEST(ProcessorSet, isset) {
  ProcessorSet processor_set;
  processor_set.set(0);
  ASSERT_TRUE(processor_set.isset(0));
  ASSERT_FALSE(processor_set.isset(1));
}

TEST(ProcessorSet, set) {
  ProcessorSet processor_set;
  processor_set.set(0);
  ASSERT_TRUE(processor_set.isset(0));
  ASSERT_FALSE(processor_set.isset(1));
}
}
}
