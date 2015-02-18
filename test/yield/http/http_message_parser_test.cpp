// http_message_parser_test.cpp

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

class TestParseCallbacks : public HttpRequestParser::ParseCallbacks {
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

TEST(HttpMessageParser, MalformedFieldMissingColon) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nHost\r\n\r\n");
  TestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_request_);
}

TEST(HttpMessageParser, MalformedFieldMissingName) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\n: localhost\r\n\r\n");
  TestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_request_);
}

TEST(HttpMessageParser, WellFormedChunkedBodyWithChunkExtension) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n1;chunk_ext1;chunk_ext2=\"ChunkExtension\"\r\nx\r\n0\r\n\r\n");
  TestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_request_);
  ASSERT_EQ(callbacks.http_request_->http_version(), 1);
  {
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_message_body_chunk_);
  ASSERT_EQ(callbacks.http_message_body_chunk_->size(), 1);
  }
  {
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_message_body_chunk_);
  ASSERT_EQ(callbacks.http_message_body_chunk_->size(), 0);
  }
}

TEST(HttpMessageParser, WellFormedChunkedBodyWithTrailer) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n1\r\nx\r\n1\r\ny\r\n0\r\nHost: localhost\r\nX-Host: x-localhost\r\n\r\n");
  TestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_request_);
  ASSERT_EQ(callbacks.http_request_->http_version(), 1);
  {
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_message_body_chunk_);
  ASSERT_EQ(callbacks.http_message_body_chunk_->size(), 1);
  }
  {
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_message_body_chunk_);
  ASSERT_EQ(callbacks.http_message_body_chunk_->size(), 1);
  }
  {
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_message_body_chunk_);
  ASSERT_EQ(callbacks.http_message_body_chunk_->size(), 0);
  }
}

TEST(HttpMessageParser, WellFormed1ChunkBody) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n1\r\nx\r\n0\r\n\r\n");
  TestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_request_);
  ASSERT_EQ(callbacks.http_request_->http_version(), 1);
  ASSERT_EQ((*callbacks.http_request_)["Host"], "localhost");
  {
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_message_body_chunk_);
  ASSERT_EQ(callbacks.http_message_body_chunk_->size(), 1);
  }
  {
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_message_body_chunk_);
  ASSERT_EQ(callbacks.http_message_body_chunk_->size(), 0);
  }
}

TEST(HttpMessageParser, WellFormed2ChunkBody) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n1\r\nx\r\n1\r\ny\r\n0\r\n\r\n");
  TestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_request_);
  ASSERT_EQ(callbacks.http_request_->http_version(), 1);
  ASSERT_EQ((*callbacks.http_request_)["Host"], "localhost");
  {
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_message_body_chunk_);
  ASSERT_EQ(callbacks.http_message_body_chunk_->size(), 1);
  }
  {
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_message_body_chunk_);
  ASSERT_EQ(callbacks.http_message_body_chunk_->size(), 1);
  }
  {
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_message_body_chunk_);
  ASSERT_EQ(callbacks.http_message_body_chunk_->size(), 0);
  }
}

TEST(HttpMessageParser, WellFormedFieldMissingValue) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nHost:\r\n\r\n");
  TestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_request_);
  ASSERT_EQ(callbacks.http_request_->http_version(), 1);
  ASSERT_EQ((*callbacks.http_request_)["Host"], "");
}

TEST(HttpMessageParser, WellFormedNoBody) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
  TestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_request_);
  ASSERT_FALSE(callbacks.http_request_->body_buffer());
}

TEST(HttpMessageParser, WellFormedNormalBody) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\n12");
  TestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_request_);
  ASSERT_TRUE(callbacks.http_request_->body_buffer());
  ASSERT_EQ(callbacks.http_request_->body_buffer()->size(), 2u);
}

TEST(HttpMessageParser, WellFormedPipelinedNoBody) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\n\r\nGET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
  {
  TestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_request_);
  ASSERT_EQ(callbacks.http_request_->http_version(), 1);
  ASSERT_EQ((*callbacks.http_request_)["Host"], "localhost");
  ASSERT_FALSE(callbacks.http_request_->body_buffer());
  }
  {
  TestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_request_);
  ASSERT_EQ(callbacks.http_request_->http_version(), 1);
  ASSERT_EQ((*callbacks.http_request_)["Host"], "localhost");
  ASSERT_FALSE(callbacks.http_request_->body_buffer());
  }
}

TEST(HttpMessageParser, WellFormedPipelinedNormalBody) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\n12GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\n12");
  {
  TestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_request_);
  ASSERT_EQ(callbacks.http_request_->http_version(), 1);
  ASSERT_EQ((*callbacks.http_request_)["Host"], "localhost");
  ASSERT_TRUE(callbacks.http_request_->body_buffer());
  ASSERT_EQ(callbacks.http_request_->body_buffer()->size(), 2u);
  }
  {
  TestParseCallbacks callbacks;
  http_request_parser.parse(callbacks);
  ASSERT_TRUE(callbacks.http_request_);
  ASSERT_EQ(callbacks.http_request_->http_version(), 1);
  ASSERT_EQ((*callbacks.http_request_)["Host"], "localhost");
  ASSERT_TRUE(callbacks.http_request_->body_buffer());
  ASSERT_EQ(callbacks.http_request_->body_buffer()->size(), 2u);
  }
}
}
}
