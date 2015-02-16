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

#include "yield/http/server/file/http_file_server_event_handler.hpp"

#include "yield/logging.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/http/server/http_server_request.hpp"

namespace yield {
namespace http {
namespace server {
namespace file {
using ::std::shared_ptr;
using ::std::unique_ptr;
using ::yield::fs::File;
using ::yield::fs::FileSystem;
using ::yield::fs::Path;
using ::yield::uri::Uri;

HttpFileServerEventHandler::HttpFileServerEventHandler(
  const Path& root_directory_path,
  const Uri& root_uri
) : root_directory_path(root_directory_path),
  root_uri(root_uri) {
  this->root_uri.get_path(root_uri_path);
}

void HttpFileServerEventHandler::handle(unique_ptr<HttpServerEvent> event) {
  if (event->type() != HttpServerEvent::Type::REQUEST) {
    return;
  }

  unique_ptr<HttpServerRequest> http_request(static_cast<HttpServerRequest*>(event.release()));
  iovec http_request_uri_path;
  http_request->uri().get_path(http_request_uri_path);

  if (http_request_uri_path.iov_len > root_uri_path.iov_len) {
    if (
      memcmp(
        http_request_uri_path.iov_base,
        root_uri_path.iov_base,
        root_uri_path.iov_len
      ) == 0
    ) {
      Path file_relpath(
        static_cast<char*>(http_request_uri_path.iov_base) +
        root_uri_path.iov_len,
        http_request_uri_path.iov_len - root_uri_path.iov_len
      );

      Path file_abspath = root_directory_path / file_relpath;

      unique_ptr<File> file = FileSystem().open(file_abspath);
      if (file != NULL) {
        unique_ptr<File::Map> map = file->mmap(SIZE_MAX, 0, true);
        if (map != NULL) {
          http_request->respond(200, shared_ptr<File::Map>(map.release()));
        } else {
          http_request->respond(404, Exception());
        }
      } else {
        http_request->respond(404, Exception());
      }
    }
  }
}
}
}
}
}
