// tstring_test.cpp

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

#include "yield/i18n/tstring.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace i18n {
using ::std::string;

TEST(tstring, encode) {
  tstring ts(
    "\304rger mit b\366sen B\374bchen ohne Augenma\337",
    Code::ISO88591
  );
  string s = ts.encode(Code::UTF8);
  ASSERT_EQ(
    s,
    "\303\204rger mit b\303\266sen B\303\274bchen ohne Augenma\303\237"
  );
}

TEST(tstring, init_from_char) {
  tstring ts('t');
  ASSERT_EQ(ts.size(), 1);
}

TEST(tstring, init_from_c_string) {
  tstring ts("tstring");
  ASSERT_EQ(ts.size(), 7);
}

TEST(tstring, init_from_iovec) {
  tstring ts("tstring", 6);
  ASSERT_EQ(ts.size(), 6);
}

TEST(tstring, init_from_string) {
  tstring ts(string("tstring"));
  ASSERT_EQ(ts.size(), 7);
}

#ifdef _WIN32
TEST(tstring, init_from_wchar) {
  tstring ts(L't');
  ASSERT_EQ(ts.size(), 1);
}

TEST(tstring, init_from_w_string) {
  tstring ts(L"tstring");
  ASSERT_EQ(ts.size(), 7);
}

TEST(tstring, init_from_w_iovec) {
  tstring ts(L"tstring", 6);
  ASSERT_EQ(ts.size(), 6);
}

TEST(tstring, init_from_wstring) {
  tstring ts(std::wstring(L"tstring"));
  ASSERT_EQ(ts.size(), 7);
}
#endif
}
}
