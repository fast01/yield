// uri_test.cpp

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

#include "yield/auto_object.hpp"
#include "yield/uri/uri.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace uri {
TEST(URI, copy_constructor) {
  URI uri("http://minorg:minorg@localhost:80/myfile");
  URI uri_copy(uri);
  ASSERT_EQ(uri_copy.get_scheme(), "http");
  //ASSERT_EQ(uri_copy.get_user(), "minorg");
  //ASSERT_EQ(uri_copy.get_password(), "minorg");
  ASSERT_EQ(uri_copy.get_host(), "localhost");
  ASSERT_EQ(uri_copy.get_port(), 80);
  ASSERT_EQ(uri_copy.get_path(), "/myfile");
}

TEST(URI, set_port) {
  URI uri("http://localhost/");
  ASSERT_EQ(uri.get_port(), 0);
  uri.set_port(80);
  ASSERT_EQ(uri.get_port(), 80);
}
}
}
