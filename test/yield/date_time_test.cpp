// date_time_test.cpp

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

#include "yield/date_time.hpp"
#include "gtest/gtest.h"

#include <sstream> // For std::ostringstream
#ifdef _WIN32
#include <Windows.h> // For FILETIME
#endif

namespace yield {
using ::std::string;

#ifndef _WIN32
TEST(DateTime, as_timeval) {
  DateTime date_time(DateTime::now());
  timeval tv = date_time;

  ASSERT_EQ(
    static_cast<long>(date_time.as_unix_date_time_s()),
    tv.tv_sec
  );
  ASSERT_LT(tv.tv_usec, 1000000);
}

TEST(DateTime, as_local_tm) {
  DateTime date_time(DateTime::now());
  tm tm_ = date_time;
  ASSERT_GE(tm_.tm_sec, 0);
  ASSERT_LE(tm_.tm_sec, 59);
  ASSERT_GE(tm_.tm_min, 0);
  ASSERT_LE(tm_.tm_min, 59);
  ASSERT_GE(tm_.tm_hour, 0);
  ASSERT_LE(tm_.tm_hour, 23);
  ASSERT_GE(tm_.tm_mday, 1);
  ASSERT_LE(tm_.tm_mday, 31);
  ASSERT_GE(tm_.tm_mon, 0);
  ASSERT_LE(tm_.tm_mon, 11);
  ASSERT_GE(tm_.tm_year, 0); // Current year - 1900
  ASSERT_GE(tm_.tm_wday, 0);
  ASSERT_LE(tm_.tm_wday, 6);
  ASSERT_GE(tm_.tm_yday, 0);
  ASSERT_LE(tm_.tm_yday, 365);
}

TEST(DateTime, as_utc_tm) {
  DateTime date_time(DateTime::now());
  tm tm_ = date_time;
  ASSERT_GE(tm_.tm_sec, 0);
  ASSERT_LE(tm_.tm_sec, 59);
  ASSERT_GE(tm_.tm_min, 0);
  ASSERT_LE(tm_.tm_min, 59);
  ASSERT_GE(tm_.tm_hour, 0);
  ASSERT_LE(tm_.tm_hour, 23);
  ASSERT_GE(tm_.tm_mday, 1);
  ASSERT_LE(tm_.tm_mday, 31);
  ASSERT_GE(tm_.tm_mon, 0);
  ASSERT_LE(tm_.tm_mon, 11);
  ASSERT_GE(tm_.tm_year, 0); // Current year - 1900
  ASSERT_GE(tm_.tm_wday, 0);
  ASSERT_LE(tm_.tm_wday, 6);
  ASSERT_GE(tm_.tm_yday, 0);
  ASSERT_LE(tm_.tm_yday, 365);
}
#endif

TEST(DateTime, as_string) {
  string date_time = DateTime::now();
  ASSERT_FALSE(date_time.empty());
  ASSERT_NE(date_time.find_first_of("20"), string::npos);
}

TEST(DateTime, copy) {
  DateTime date_time(DateTime::now());
  DateTime date_time_copy(date_time);
  ASSERT_EQ(date_time.as_unix_date_time_ns(), date_time_copy.as_unix_date_time_ns());
}

#ifndef _WIN32
TEST(DateTime, from_timeval) {
  DateTime date_time(DateTime::now());
  timeval tv = date_time;
  DateTime timeout_copy(tv);
  uint64_t inaccuracy_ns
  = date_time.as_unix_date_time_ns() - timeout_copy.as_unix_date_time_ns();
  ASSERT_LT(inaccuracy_ns, Time::NS_IN_S);
}

TEST(DateTime, from_local_tm) {
  DateTime date_time1(DateTime::now());
  tm tm1 = date_time1.as_local_tm();
  DateTime date_time2(tm1, true);
  ASSERT_EQ(date_time1, date_time2);
}

TEST(DateTime, from_utc_tm) {
  DateTime date_time1(DateTime::now());
  tm tm1 = date_time1.as_utc_tm();
  DateTime date_time2(tm1, false);
  ASSERT_EQ(date_time1, date_time2);
}
#endif

TEST(DateTime, from_tm_parts) {
  DateTime date_time(30, 30, 12, 15, 11, 80, true);

#ifndef _WIN32
  tm tm_ = date_time;
  ASSERT_EQ(tm_.tm_sec, 30);
  ASSERT_EQ(tm_.tm_min, 30);
  ASSERT_EQ(tm_.tm_mday, 15);
  ASSERT_EQ(tm_.tm_mon, 11);
  ASSERT_EQ(tm_.tm_year, 80);
  ASSERT_GE(tm_.tm_wday, 0);
  ASSERT_LE(tm_.tm_wday, 6);
  ASSERT_GE(tm_.tm_yday, 0);
  ASSERT_LE(tm_.tm_yday, 365);
#endif
}

TEST(DateTime, from_unix_time_ns) {
  DateTime date_time(static_cast<uint64_t>(1));
  ASSERT_EQ(date_time.as_unix_date_time_ns(), 1);
}

TEST(DateTime, from_unix_time_s) {
  DateTime date_time(1.0);
  ASSERT_EQ(date_time.as_unix_date_time_ms(), 1000);
  ASSERT_EQ(date_time.as_unix_date_time_us(), 1000000);
  ASSERT_EQ(date_time.as_unix_date_time_s(), 1.0);
}

TEST(DateTime, now) {
  DateTime date_time1(DateTime::now());
  ASSERT_GT(static_cast<uint64_t>(date_time1), 0);
  for (uint32_t i = 0; i < UINT32_MAX / 16; i++) {
    ;
  }
  DateTime date_time2(DateTime::now());
  ASSERT_GT(date_time2, date_time1);
}

TEST(DateTime, print) {
  std::ostringstream date_time;
  date_time << DateTime::now();
  ASSERT_FALSE(date_time.str().empty());
}
}
