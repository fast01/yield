// uri_parser_test.cpp

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

#include "yield/uri/uri.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace uri {
TEST(UriParser, path1) {
  Uri uri("/mydir/");
  ASSERT_EQ(uri.get_path(), "/mydir/");
}

TEST(UriParser, path2) {
  Uri uri("/mypath");
  ASSERT_EQ(uri.get_path(), "/mypath");
}

TEST(UriParser, path3) {
  Uri uri("/mydir/mypath");
  ASSERT_EQ(uri.get_path(), "/mydir/mypath");
}

TEST(UriParser, path_query1) {
  Uri uri("/mypath?key=value");
  ASSERT_EQ(uri.get_path(), "/mypath");
  //ASSERT_EQ(uri.get_query_value("key" ), "value");
  //ASSERT_EQ(uri.get_query_value("otherkey" ), "");
  //ASSERT_TRUE_ne(uri.get_query_values("key" ), uri.get_query().end());
}

TEST(UriParser, path_query2) {
  Uri uri("/mypath?key1=value1&key2=value2");
  ASSERT_EQ(uri.get_path(), "/mypath");
  //ASSERT_EQ(uri.get_query_value("key1" ), "value1");
  //ASSERT_EQ(uri.get_query_value("key2" ), "value2");
  //ASSERT_EQ(uri.get_query_value("otherkey" ), "");
}

TEST(UriParser, path_query3) { // A Django Uri 20110427
  Uri uri("/accounts/login/?next=/");
  ASSERT_EQ(uri.get_path(), "/accounts/login/");
  ASSERT_EQ(uri.get_query(), "next=/");
}

TEST(UriParser, scheme_host1) {
  Uri uri("http://localhost");
  ASSERT_EQ(uri.get_scheme(), "http");
  ASSERT_EQ(uri.get_host(), "localhost");
  //ASSERT_TRUE(uri.get_user().empty());
  //ASSERT_TRUE(uri.get_password().empty());
  ASSERT_EQ(uri.get_port(), 0);
  ASSERT_TRUE(uri.get_path().empty());
}

TEST(UriParser, scheme_host2) {
  Uri uri("http://localhost/");
  ASSERT_EQ(uri.get_scheme(), "http");
  ASSERT_EQ(uri.get_host(), "localhost");
  ASSERT_EQ(uri.get_path(), "/");
}

TEST(UriParser, scheme_host_port1) {
  Uri uri("http://*:80/");
  ASSERT_EQ(uri.get_scheme(), "http");
  ASSERT_EQ(uri.get_host(), "*");
  ASSERT_EQ(uri.get_path(), "/");
}

TEST(UriParser, scheme_host_port2) {
  Uri uri("http://localhost:1");
  ASSERT_EQ(uri.get_scheme(), "http");
  ASSERT_EQ(uri.get_host(), "localhost");
  ASSERT_EQ(uri.get_port(), 1);
  ASSERT_TRUE(uri.get_path().empty());
}

TEST(UriParser, scheme_host_port3) {
  Uri uri("http://localhost:1/");
  ASSERT_EQ(uri.get_scheme(), "http");
  ASSERT_EQ(uri.get_host(), "localhost");
  ASSERT_EQ(uri.get_port(), 1);
  ASSERT_EQ(uri.get_path(), "/");
}

TEST(UriParser, scheme_user_host) {
  Uri uri("http://minorg@localhost");
  ASSERT_EQ(uri.get_scheme(), "http");
  ASSERT_EQ(uri.get_host(), "localhost");
  //ASSERT_EQ(uri.get_user(), "minorg");
  //ASSERT_TRUE(uri.get_password().empty());
}

TEST(UriParser, scheme_user_password_host) {
  Uri uri("http://minorg:minorg@localhost");
  ASSERT_EQ(uri.get_scheme(), "http");
  ASSERT_EQ(uri.get_host(), "localhost");
  //ASSERT_EQ(uri.get_user(), "minorg");
  //ASSERT_EQ(uri.get_password(), "minorg");
}
}
}
