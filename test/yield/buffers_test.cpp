// buffers_test.cpp

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

#include "yield/buffer.hpp"
#include "yield/buffers.hpp"
#include "gtest/gtest.h"

bool operator==(const iovec& left, const iovec& right) {
  return left.iov_base == right.iov_base
         &&
         left.iov_len == right.iov_len;
}

namespace yield {
using ::std::make_shared;
using ::std::shared_ptr;
using ::std::unique_ptr;
using ::std::vector;

TEST(Buffers, as_read_iovecs) {
  unique_ptr<Buffer> buffers(new Buffer(Buffer::getpagesize()));
  buffers->set_next_buffer(make_shared<Buffer>(Buffer::getpagesize()));
  vector<iovec> read_iovecs;
  Buffers::as_read_iovecs(*buffers, read_iovecs);
  ASSERT_EQ(read_iovecs[0], buffers->as_read_iovec());
  ASSERT_EQ(read_iovecs[1], buffers->next_buffer()->as_read_iovec());
}

TEST(Buffers, as_write_iovecs) {
  unique_ptr<Buffer> buffers(new Buffer(Buffer::getpagesize()));
  buffers->set_next_buffer(make_shared<Buffer>(Buffer::getpagesize()));
  vector<iovec> write_iovecs;
  Buffers::as_write_iovecs(*buffers, write_iovecs);
  ASSERT_EQ(write_iovecs.size(), 2);
  ASSERT_EQ(write_iovecs[0], buffers->as_write_iovec());
  ASSERT_EQ(write_iovecs[1], buffers->next_buffer()->as_write_iovec());
}

TEST(Buffers, as_write_iovecs_offset) {
  shared_ptr<Buffer> buffers = Buffer::copy("test ");
  buffers->set_next_buffer(Buffer::copy("string"));
  vector<iovec> write_iovecs;

  Buffers::as_write_iovecs(*buffers, 0, write_iovecs);
  ASSERT_EQ(write_iovecs.size(), 2);
  ASSERT_EQ(write_iovecs[0], buffers->as_write_iovec());
  ASSERT_EQ(write_iovecs[1], buffers->next_buffer()->as_write_iovec());
  write_iovecs.clear();

  Buffers::as_write_iovecs(*buffers, 1, write_iovecs);
  ASSERT_EQ(write_iovecs.size(), 2);
  ASSERT_EQ(write_iovecs[0].iov_base, static_cast<char*>(*buffers) + 1);
  ASSERT_EQ(write_iovecs[0].iov_len, buffers->size() - 1);
  ASSERT_EQ(write_iovecs[1], buffers->next_buffer()->as_write_iovec());
  write_iovecs.clear();

  Buffers::as_write_iovecs(*buffers, 5, write_iovecs);
  ASSERT_EQ(write_iovecs.size(), 1);
  ASSERT_EQ(write_iovecs[0], buffers->next_buffer()->as_write_iovec());
  write_iovecs.clear();

  Buffers::as_write_iovecs(*buffers, 6, write_iovecs);
  ASSERT_EQ(write_iovecs.size(), 1);
  ASSERT_EQ(
    write_iovecs[0].iov_base,
    static_cast<char*>(*buffers->next_buffer()) + 1
  );
  ASSERT_EQ(
    write_iovecs[0].iov_len,
    buffers->next_buffer()->size() - 1
  );
  write_iovecs.clear();

  Buffers::as_write_iovecs(*buffers, 11, write_iovecs);
  ASSERT_TRUE(write_iovecs.empty());
  Buffers::as_write_iovecs(*buffers, 12, write_iovecs);
  ASSERT_TRUE(write_iovecs.empty());
}

TEST(Buffers, put) {
  unique_ptr<Buffer> buffers(new Buffer(Buffer::getpagesize()));
  buffers->set_next_buffer(make_shared<Buffer>(Buffer::getpagesize()));
  Buffers::put(*buffers, NULL, Buffer::getpagesize() / 2);
  ASSERT_EQ(Buffers::size(*buffers), Buffer::getpagesize() / 2);
  Buffers::put(*buffers, NULL, Buffer::getpagesize() / 2);
  ASSERT_EQ(Buffers::size(*buffers), Buffer::getpagesize());
  Buffers::put(*buffers, NULL, Buffer::getpagesize() / 2);
  ASSERT_EQ(
    Buffers::size(*buffers),
    Buffer::getpagesize() + Buffer::getpagesize() / 2
  );
}

TEST(Buffers, size) {
  unique_ptr<Buffer> buffers(new Buffer(Buffer::getpagesize()));
  buffers->set_next_buffer(make_shared<Buffer>(Buffer::getpagesize()));
  Buffers::put(*buffers, NULL, Buffer::getpagesize() / 2);
  ASSERT_EQ(Buffers::size(*buffers), Buffer::getpagesize() / 2);
  Buffers::put(*buffers, NULL, Buffer::getpagesize() / 2);
  ASSERT_EQ(Buffers::size(*buffers), Buffer::getpagesize());
  Buffers::put(*buffers, NULL, Buffer::getpagesize() / 2);
  ASSERT_EQ(
    Buffers::size(*buffers),
    Buffer::getpagesize() + Buffer::getpagesize() / 2
  );
}
}
