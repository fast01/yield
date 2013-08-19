// fs_event_test.cpp

// Copyright (c) 2013 Minor Gordon
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

#include "yield/fs/poll/fs_event.hpp"
#include "gtest/gtest.h"

#include <sstream>

namespace yield {
namespace fs {
namespace poll {
TEST(FsEvent, constructors) {
  FsEvent("path", FsEvent::TYPE_DIRECTORY_ADD);
  FsEvent("old_path", "new_path", FsEvent::TYPE_DIRECTORY_RENAME);
}

TEST(FsEvent, get_new_path) {
  ASSERT_EQ(
    FsEvent("old_path", "new_path", FsEvent::TYPE_FILE_RENAME).get_new_path(),
    Path("new_path")
  );
}

TEST(FsEvent, get_old_path) {
  ASSERT_EQ(
    FsEvent("old_path", "new_path", FsEvent::TYPE_FILE_RENAME).get_old_path(),
    Path("old_path")
  );
}

TEST(FsEvent, get_path) {
  ASSERT_EQ(
    FsEvent("path", FsEvent::TYPE_FILE_ADD).get_path(),
    Path("path")
  );
}

TEST(FsEvent, get_type) {
  ASSERT_EQ(
    FsEvent("path", FsEvent::TYPE_FILE_ADD).get_type(),
    FsEvent::TYPE_FILE_ADD
  );
}

TEST(FsEvent, get_type_id) {
  ASSERT_EQ(
    FsEvent("path", FsEvent::TYPE_FILE_ADD).get_type_id(),
    FsEvent::TYPE_ID
  );
}

TEST(FsEvent, get_type_name) {
  ASSERT_EQ(
    strcmp(
      FsEvent("path", FsEvent::TYPE_FILE_ADD).get_type_name(),
      "yield::fs::poll::FsEvent"
    ),
    0
  );
}

TEST(FsEvent, print) {
  std::ostringstream oss;
  oss << FsEvent("path", FsEvent::TYPE_FILE_ADD);
  ASSERT_NE(oss.str().size(), 0u);
}
}
}
}
