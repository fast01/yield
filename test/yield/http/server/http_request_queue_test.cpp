// http_request_queue_test.cpp

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

#include "../../event_queue_test.hpp"
#include "yield/auto_object.hpp"
#include "yield/logging.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/stat.hpp"
#include "yield/http/http_response.hpp"
#include "yield/http/server/http_request.hpp"
#include "yield/http/server/http_request_queue.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace http {
namespace server {
class TestHttpRequestQueue : public HttpRequestQueue<> {
public:
  TestHttpRequestQueue()
    : HttpRequestQueue<>(8000) {
  }
};

INSTANTIATE_TYPED_TEST_CASE_P(HttpRequestQueue, EventQueueTest, TestHttpRequestQueue);

class HttpRequestQueueTest : public ::testing::Test {
protected:
  void handle(HttpRequest& http_request) {
    if (http_request.uri().get_path() == "/") {
      http_request.respond(200, "Hello world");
    } else if (http_request.uri().get_path() == "/drop") {
      ;
    } else if (http_request.uri().get_path() == "/sendfile") {
      yield::fs::File* file
#ifdef _WIN32
      = yield::fs::FileSystem().open("yield.http.server.Makefile");
#else
      = yield::fs::FileSystem().open("Makefile");
#endif
      if (file != NULL) {
        yield::fs::Stat* stbuf = file->stat();
        if (stbuf != NULL) {
          HttpResponse* http_response = new HttpResponse(200, file);
          http_response->set_field(
            "Content-Length",
            static_cast<size_t>(stbuf->get_size())
          );
          yield::fs::Stat::dec_ref(*stbuf);
          http_request.respond(*http_response);
        } else {
          yield::fs::File::dec_ref(*file);
          http_request.respond(404);
        }
      } else {
        http_request.respond(404);
      }
    } else {
      http_request.respond(404);
    }

    HttpRequest::dec_ref(http_request);
  }
};

TEST_F(HttpRequestQueueTest, dequeue) {
  TestHttpRequestQueue http_request_queue;
  for (;;) {
    HttpRequest* http_request
    = Object::cast<HttpRequest>(http_request_queue.timeddequeue(30.0));
    if (http_request != NULL) {
      handle(*http_request);
    } else {
      break;
    }
  }
}
}
}
}
