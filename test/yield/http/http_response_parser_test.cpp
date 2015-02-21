// http_response_parser_test.cpp

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
#include "yield/http/http_response.hpp"
#include "yield/http/http_response_parser.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace http {
using ::std::unique_ptr;

class TestResponseParseCallbacks final : public HttpResponseParser::ParseCallbacks {
public:
  void handle_http_message_body_chunk(::std::shared_ptr<Buffer>) override {
  }

  void
  handle_http_response(
    ::std::shared_ptr<Buffer> body,
    uint8_t http_version,
    uint16_t status_code
  ) override {
    http_response_.reset(new HttpResponse(http_version, status_code, body));
  }

  void
  handle_http_response(
    ::std::shared_ptr<Buffer> body,
    uint16_t fields_offset,
    ::std::shared_ptr<Buffer> header,
    uint8_t http_version,
    uint16_t status_code
  ) override {
    http_response_.reset(new HttpResponse(body, fields_offset, header, http_version, status_code));
  }

  void read(::std::shared_ptr<Buffer>) override {
  }

public:
  unique_ptr<HttpResponse> http_response_;
  unique_ptr<HttpMessageBodyChunk> http_message_body_chunk_;
};


TEST(HttpResponseParser, MalformedReasonPhraseMissing) {
  HttpResponseParser http_response_parser("HTTP/1.1 200\r\n\r\n");
  TestResponseParseCallbacks callbacks;
  http_response_parser.parse(callbacks);
  ASSERT_FALSE(static_cast<bool>(callbacks.http_response_));
  //ASSERT_EQ(callbacks.http_response_->status_code(), 400);
}

TEST(HttpResponseParser, MalformedStatusCodeAlpha) {
  HttpResponseParser http_response_parser("HTTP/1.1 XX OK\r\n\r\n");
  TestResponseParseCallbacks callbacks;
  http_response_parser.parse(callbacks);
  ASSERT_FALSE(static_cast<bool>(callbacks.http_response_));
  //ASSERT_EQ(callbacks.http_response_->status_code(), 400);
}

TEST(HttpResponseParser, MalformedStatusCodeMissing) {
  HttpResponseParser http_response_parser("HTTP/1.1 OK\r\n\r\n");
  TestResponseParseCallbacks callbacks;
  http_response_parser.parse(callbacks);
  ASSERT_FALSE(static_cast<bool>(callbacks.http_response_));
  //ASSERT_EQ(callbacks.http_response_->status_code(), 400);
}

TEST(HttpResponseParser, MalformedStatusLineMissing) {
  HttpResponseParser http_response_parser("Host: localhost\r\n\r\n");
  TestResponseParseCallbacks callbacks;
  http_response_parser.parse(callbacks);
  ASSERT_FALSE(static_cast<bool>(callbacks.http_response_));
  //ASSERT_EQ(callbacks.http_response_->status_code(), 400);
}

TEST(HttpResponseParser, WellFormedStatusLineOnly) {
  HttpResponseParser http_response_parser("HTTP/1.1 200 OK\r\n\r\n");
  TestResponseParseCallbacks callbacks;
  http_response_parser.parse(callbacks);
  ASSERT_TRUE(static_cast<bool>(callbacks.http_response_));
  ASSERT_EQ(callbacks.http_response_->status_code(), 200);
  ASSERT_FALSE(callbacks.http_response_->body_buffer());
}
}
}
