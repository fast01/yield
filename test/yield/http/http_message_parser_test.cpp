// http_message_parser_test.cpp

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
#include "yield/buffer.hpp"
#include "yield/http/http_message_body_chunk.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_request_parser.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace http {
TEST(HttpMessageParser, MalformedFieldMissingColon) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nHost\r\n\r\n");
  HttpRequest* http_request = Object::cast<HttpRequest>(http_request_parser.parse());
  ASSERT_EQ(http_request, static_cast<Object*>(NULL));
}

TEST(HttpMessageParser, MalformedFieldMissingName) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\n: localhost\r\n\r\n");
  HttpRequest* http_request = Object::cast<HttpRequest>(http_request_parser.parse());
  ASSERT_EQ(http_request, static_cast<Object*>(NULL));
}

TEST(HttpMessageParser, WellFormedChunkedBodyWithChunkExtension) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n1;chunk_ext1;chunk_ext2=\"ChunkExtension\"\r\nx\r\n0\r\n\r\n");
  HttpRequest* http_request = Object::cast<HttpRequest>(http_request_parser.parse());
  ASSERT_NE(http_request, static_cast<Object*>(NULL));
  ASSERT_EQ(http_request->get_http_version(), 1);
  {
  HttpMessageBodyChunk* http_message_body_chunk = Object::cast<HttpMessageBodyChunk>(http_request_parser.parse());
  ASSERT_NE(http_message_body_chunk, static_cast<Object*>(NULL));
  ASSERT_EQ(http_message_body_chunk->size(), 1);
  HttpMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  {
  HttpMessageBodyChunk* http_message_body_chunk = Object::cast<HttpMessageBodyChunk>(http_request_parser.parse());
  ASSERT_NE(http_message_body_chunk, static_cast<Object*>(NULL));
  ASSERT_EQ(http_message_body_chunk->size(), 0);
  HttpMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  HttpRequest::dec_ref(http_request);
}

TEST(HttpMessageParser, WellFormedChunkedBodyWithTrailer) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n1\r\nx\r\n1\r\ny\r\n0\r\nHost: localhost\r\nX-Host: x-localhost\r\n\r\n");
  HttpRequest* http_request = Object::cast<HttpRequest>(http_request_parser.parse());
  ASSERT_NE(http_request, static_cast<Object*>(NULL));
  ASSERT_EQ(http_request->get_http_version(), 1);
  {
  HttpMessageBodyChunk* http_message_body_chunk = Object::cast<HttpMessageBodyChunk>(http_request_parser.parse());
  ASSERT_NE(http_message_body_chunk, static_cast<Object*>(NULL));
  ASSERT_EQ(http_message_body_chunk->size(), 1);
  HttpMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  {
  HttpMessageBodyChunk* http_message_body_chunk = Object::cast<HttpMessageBodyChunk>(http_request_parser.parse());
  ASSERT_NE(http_message_body_chunk, static_cast<Object*>(NULL));
  ASSERT_EQ(http_message_body_chunk->size(), 1);
  HttpMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  {
  HttpMessageBodyChunk* http_message_body_chunk = Object::cast<HttpMessageBodyChunk>(http_request_parser.parse());
  ASSERT_NE(http_message_body_chunk, static_cast<Object*>(NULL));
  ASSERT_EQ(http_message_body_chunk->size(), 0);
  HttpMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  HttpRequest::dec_ref(http_request);
}

TEST(HttpMessageParser, WellFormed1ChunkBody) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n1\r\nx\r\n0\r\n\r\n");
  HttpRequest* http_request = Object::cast<HttpRequest>(http_request_parser.parse());
  ASSERT_NE(http_request, static_cast<Object*>(NULL));
  ASSERT_EQ(http_request->get_http_version(), 1);
  ASSERT_EQ((*http_request)["Host"], "localhost");
  {
  HttpMessageBodyChunk* http_message_body_chunk = Object::cast<HttpMessageBodyChunk>(http_request_parser.parse());
  ASSERT_NE(http_message_body_chunk, static_cast<Object*>(NULL));
  ASSERT_EQ(http_message_body_chunk->size(), 1);
  HttpMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  {
  HttpMessageBodyChunk* http_message_body_chunk = Object::cast<HttpMessageBodyChunk>(http_request_parser.parse());
  ASSERT_NE(http_message_body_chunk, static_cast<Object*>(NULL));
  ASSERT_EQ(http_message_body_chunk->size(), 0);
  HttpMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  HttpRequest::dec_ref(http_request);
}

TEST(HttpMessageParser, WellFormed2ChunkBody) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n1\r\nx\r\n1\r\ny\r\n0\r\n\r\n");
  HttpRequest* http_request = Object::cast<HttpRequest>(http_request_parser.parse());
  ASSERT_NE(http_request, static_cast<Object*>(NULL));
  ASSERT_EQ(http_request->get_http_version(), 1);
  ASSERT_EQ((*http_request)["Host"], "localhost");
  {
  HttpMessageBodyChunk* http_message_body_chunk = Object::cast<HttpMessageBodyChunk>(http_request_parser.parse());
  ASSERT_NE(http_message_body_chunk, static_cast<Object*>(NULL));
  ASSERT_EQ(http_message_body_chunk->size(), 1);
  HttpMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  {
  HttpMessageBodyChunk* http_message_body_chunk = Object::cast<HttpMessageBodyChunk>(http_request_parser.parse());
  ASSERT_NE(http_message_body_chunk, static_cast<Object*>(NULL));
  ASSERT_EQ(http_message_body_chunk->size(), 1);
  HttpMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  {
  HttpMessageBodyChunk* http_message_body_chunk = Object::cast<HttpMessageBodyChunk>(http_request_parser.parse());
  ASSERT_NE(http_message_body_chunk, static_cast<Object*>(NULL));
  ASSERT_EQ(http_message_body_chunk->size(), 0);
  HttpMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  HttpRequest::dec_ref(http_request);
}

TEST(HttpMessageParser, WellFormedFieldMissingValue) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nHost:\r\n\r\n");
  HttpRequest* http_request = Object::cast<HttpRequest>(http_request_parser.parse());
  ASSERT_NE(http_request, static_cast<Object*>(NULL));
  ASSERT_EQ(http_request->get_http_version(), 1);
  ASSERT_EQ((*http_request)["Host"], "");
  HttpRequest::dec_ref(http_request);
}

TEST(HttpMessageParser, WellFormedNoBody) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
  HttpRequest* http_request = Object::cast<HttpRequest>(http_request_parser.parse());
  ASSERT_NE(http_request, static_cast<Object*>(NULL));
  ASSERT_EQ(http_request->get_body(), static_cast<Object*>(NULL));
  HttpRequest::dec_ref(http_request);
}

TEST(HttpMessageParser, WellFormedNormalBody) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\n12");
  HttpRequest* http_request = Object::cast<HttpRequest>(http_request_parser.parse());
  ASSERT_NE(http_request, static_cast<Object*>(NULL));
  ASSERT_NE(http_request->get_body(), static_cast<Object*>(NULL));
  ASSERT_EQ(static_cast<Buffer*>(http_request->get_body())->size(), 2u);
  HttpRequest::dec_ref(http_request);
}

TEST(HttpMessageParser, WellFormedPipelinedNoBody) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\n\r\nGET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
  {
  HttpRequest* http_request = Object::cast<HttpRequest>(http_request_parser.parse());
  ASSERT_NE(http_request, static_cast<Object*>(NULL));
  ASSERT_EQ(http_request->get_http_version(), 1);
  ASSERT_EQ((*http_request)["Host"], "localhost");
  ASSERT_EQ(http_request->get_body(), static_cast<Object*>(NULL));
  HttpRequest::dec_ref(http_request);
  }
  {
  HttpRequest* http_request = Object::cast<HttpRequest>(http_request_parser.parse());
  ASSERT_NE(http_request, static_cast<Object*>(NULL));
  ASSERT_EQ(http_request->get_http_version(), 1);
  ASSERT_EQ((*http_request)["Host"], "localhost");
  ASSERT_EQ(http_request->get_body(), static_cast<Object*>(NULL));
  HttpRequest::dec_ref(http_request);
  }
}

TEST(HttpMessageParser, WellFormedPipelinedNormalBody) {
  HttpRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\n12GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\n12");
  {
  HttpRequest* http_request = Object::cast<HttpRequest>(http_request_parser.parse());
  ASSERT_NE(http_request, static_cast<Object*>(NULL));
  ASSERT_EQ(http_request->get_http_version(), 1);
  ASSERT_EQ((*http_request)["Host"], "localhost");
  ASSERT_NE(http_request->get_body(), static_cast<Object*>(NULL));
  ASSERT_EQ(static_cast<Buffer*>(http_request->get_body())->size(), 2u);
  HttpRequest::dec_ref(http_request);
  }
  {
  HttpRequest* http_request = Object::cast<HttpRequest>(http_request_parser.parse());
  ASSERT_NE(http_request, static_cast<Object*>(NULL));
  ASSERT_EQ(http_request->get_http_version(), 1);
  ASSERT_EQ((*http_request)["Host"], "localhost");
  ASSERT_NE(http_request->get_body(), static_cast<Object*>(NULL));
  ASSERT_EQ(static_cast<Buffer*>(http_request->get_body())->size(), 2u);
  HttpRequest::dec_ref(http_request);
  }
}
}
}
