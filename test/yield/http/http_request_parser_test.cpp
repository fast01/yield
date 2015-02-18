// http_request_parser_test.cpp

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
#include "yield/http/http_message_body_chunk.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_request_parser.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace http {
using ::std::move;
using ::std::unique_ptr;

class TestRequestParseCallbacks : public HttpRequestParser::ParseCallbacks {
public:
  void handle_http_request(::std::unique_ptr<HttpRequest> http_request) override {
    http_request_.swap(http_request);
  }

  void handle_error_http_response(::std::unique_ptr<HttpResponse> error_http_response) override {
    error_http_response_.swap(error_http_response);
  }

  void handle_http_message_body_chunk(::std::unique_ptr<HttpMessageBodyChunk> http_message_body_chunk) override {
    http_message_body_chunk_.swap(http_message_body_chunk);
  }

  void read(::std::shared_ptr<Buffer>) override {
  }

public:
  unique_ptr<HttpResponse> error_http_response_;
  unique_ptr<HttpMessageBodyChunk> http_message_body_chunk_;
  unique_ptr<HttpRequest> http_request_;
};


TEST(HttpRequestParser, MalformedHTTPVersionMissing) {
  HttpRequestParser http_request_parser("GET /\r\nHost: localhost\r\n\r\n");
  TestRequestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_FALSE(static_cast<bool>(callbacks.http_request_));
}

TEST(HttpRequestParser, MalformedHTTPVersionMissingHTTP) {
  HttpRequestParser http_request_parser("GET / /1.0\r\nHost: localhost\r\n\r\n");
  TestRequestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_FALSE(static_cast<bool>(callbacks.http_request_));
}

TEST(HttpRequestParser, MalformedHTTPVersionMissingMinorVersion) {
  HttpRequestParser http_request_parser("GET / HTTP/1.\r\nHost: localhost\r\n\r\n");
  TestRequestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_FALSE(static_cast<bool>(callbacks.http_request_));
}

TEST(HttpRequestParser, MalformedHTTPVersionMissingTrailingCRLF) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1Host: localhost\r\n\r\n");
  TestRequestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_FALSE(static_cast<bool>(callbacks.http_request_));
}

TEST(HttpRequestParser, MalformedMethodMissing) {
  HttpRequestParser http_request_parser("/ HTTP/1.0\r\nHost: localhost\r\n\r\n");
  TestRequestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_FALSE(static_cast<bool>(callbacks.http_request_));
}

TEST(HttpRequestParser, MalformedURIEmbeddedLF) {
  HttpRequestParser http_request_parser("GET /\r HTTP/1.1\r\nHost: localhost\r\n\r\n");
  TestRequestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_FALSE(static_cast<bool>(callbacks.http_request_));
}

TEST(HttpRequestParser, MalformedURIMissing) {
  HttpRequestParser http_request_parser("GET HTTP/1.1\r\nHost: localhost\r\n\r\n");
  TestRequestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_FALSE(static_cast<bool>(callbacks.http_request_));
}

TEST(HttpRequestParser, WellFormedRequestLineOnly) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\n\r\n");
  TestRequestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(static_cast<bool>(callbacks.http_request_));
  ASSERT_EQ(callbacks.http_request_->method(), HttpRequest::Method::GET);
  ASSERT_EQ(callbacks.http_request_->http_version(), 1);
  ASSERT_FALSE(callbacks.http_request_->body_buffer());
}
}
}
