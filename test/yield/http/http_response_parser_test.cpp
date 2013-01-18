// http_response_parser_test.cpp

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
#include "yield/http/http_response.hpp"
#include "yield/http/http_response_parser.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace http {
TEST(HTTPResponseParser, MalformedReasonPhraseMissing) {
  HTTPResponseParser http_response_parser("HTTP/1.1 200\r\n\r\n");
  HTTPResponse* http_response = Object::cast<HTTPResponse>(http_response_parser.parse());
  ASSERT_NE(http_response, static_cast<Object*>(NULL));
  ASSERT_EQ(http_response->get_status_code(), 400);
  HTTPResponse::dec_ref(http_response);
}

TEST(HTTPResponseParser, MalformedStatusCodeAlpha) {
  HTTPResponseParser http_response_parser("HTTP/1.1 XX OK\r\n\r\n");
  HTTPResponse* http_response = Object::cast<HTTPResponse>(http_response_parser.parse());
  ASSERT_NE(http_response, static_cast<Object*>(NULL));
  ASSERT_EQ(http_response->get_status_code(), 400);
  HTTPResponse::dec_ref(http_response);
}

TEST(HTTPResponseParser, MalformedStatusCodeMissing) {
  HTTPResponseParser http_response_parser("HTTP/1.1 OK\r\n\r\n");
  HTTPResponse* http_response = Object::cast<HTTPResponse>(http_response_parser.parse());
  ASSERT_NE(http_response, static_cast<Object*>(NULL));
  ASSERT_EQ(http_response->get_status_code(), 400);
  HTTPResponse::dec_ref(http_response);
}

TEST(HTTPResponseParser, MalformedStatusLineMissing) {
  HTTPResponseParser http_response_parser("Host: localhost\r\n\r\n");
  HTTPResponse* http_response = Object::cast<HTTPResponse>(http_response_parser.parse());
  ASSERT_NE(http_response, static_cast<Object*>(NULL));
  ASSERT_EQ(http_response->get_status_code(), 400);
  HTTPResponse::dec_ref(http_response);
}

TEST(HTTPResponseParser, WellFormedStatusLineOnly) {
  HTTPResponseParser http_response_parser("HTTP/1.1 200 OK\r\n\r\n");
  HTTPResponse* http_response = Object::cast<HTTPResponse>(http_response_parser.parse());
  ASSERT_NE(http_response, static_cast<Object*>(NULL));
  ASSERT_EQ(http_response->get_status_code(), 200);
  ASSERT_EQ(http_response->get_body(), static_cast<Object*>(NULL));
  HTTPResponse::dec_ref(http_response);
}
}
}
