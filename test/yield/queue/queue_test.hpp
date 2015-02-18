// queue_test.hpp

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

#ifndef _YIELD_QUEUE_QUEUE_TEST_HPP_
#define _YIELD_QUEUE_QUEUE_TEST_HPP_

#include "yield/logging.hpp"
#include "yield/types.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace queue {
using ::std::move;
using ::std::unique_ptr;

template <class TypeParam>
class QueueTest : public ::testing::Test {
};

TYPED_TEST_CASE_P(QueueTest);

TYPED_TEST_P(QueueTest, create) {
  new TypeParam();
}

TYPED_TEST_P(QueueTest, trydequeue) {
  TypeParam queue;

  unique_ptr<uint32_t> in_value(new uint32_t(1));
  ASSERT_FALSE(queue.tryenqueue(move(in_value)));
  unique_ptr<uint32_t> out_value = queue.trydequeue();
  ASSERT_TRUE(out_value);
  ASSERT_EQ(*out_value, 1);
  ASSERT_FALSE(queue.trydequeue());
}

TYPED_TEST_P(QueueTest, tryenqueue) {
  TypeParam queue;

  unique_ptr<uint32_t> in_value(new uint32_t(1));
  ASSERT_FALSE(queue.tryenqueue(move(in_value)));
}

REGISTER_TYPED_TEST_CASE_P(QueueTest, create, trydequeue, tryenqueue);
}
}

#endif
