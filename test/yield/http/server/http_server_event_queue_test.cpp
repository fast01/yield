// http_request_queue_test.cpp

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

#include "yield/logging.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/stat.hpp"
#include "yield/http/http_response.hpp"
#include "yield/http/server/http_server_request.hpp"
#include "yield/http/server/http_server_event_queue.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace http {
namespace server {
using ::std::move;
using ::std::shared_ptr;
using ::std::unique_ptr;
using ::yield::fs::File;
using ::yield::fs::FileSystem;
using ::yield::fs::Stat;

class TestHttpServerEventQueue : public HttpServerEventQueue {
public:
  TestHttpServerEventQueue()
    : HttpServerEventQueue(8000) {
  }
};

class HttpServerEventQueueTest : public ::testing::Test {
protected:
  void handle(unique_ptr<HttpServerRequest> http_request) {
    if (http_request->uri().get_path() == "/") {
      http_request->respond(200, "Hello world");
    } else if (http_request->uri().get_path() == "/drop") {
      ;
    } else if (http_request->uri().get_path() == "/sendfile") {
      unique_ptr<File> file
#ifdef _WIN32
       = FileSystem().open("yield.http.server.Makefile");
#else
      = FileSystem().open("Makefile");
#endif
      if (file) {
        unique_ptr<Stat> stbuf = file->stat();
        if (stbuf) {
          unique_ptr<HttpServerResponse> http_response(new HttpServerResponse(shared_ptr<File>(move(file)), http_request->http_version(), 200));
          http_response->set_field(
            "Content-Length",
            static_cast<size_t>(stbuf->size())
          );
          http_request->respond(move(http_response));
        } else {
          http_request->respond(404);
        }
      } else {
        http_request->respond(404);
      }
    } else {
      http_request->respond(404);
    }
  }
};

TEST_F(HttpServerEventQueueTest, dequeue) {
  TestHttpServerEventQueue http_request_queue;
  for (;;) {
    unique_ptr<HttpServerEvent> http_request = http_request_queue.timeddequeue(30.0);
    if (http_request) {
      handle(unique_ptr<HttpServerRequest>(static_cast<HttpServerRequest*>(http_request.release())));
    } else {
      break;
    }
  }
}
}
}
}
