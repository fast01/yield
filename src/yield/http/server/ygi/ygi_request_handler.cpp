// yield/http/server/ygi/ygi_request_handler.cpp

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

#include "yield/logging.hpp"
#include "yield/http/server/http_request.hpp"
#include "yield/http/server/ygi/ygi_request.hpp"
#include "yield/http/server/ygi/ygi_request_handler.hpp"

namespace yield {
namespace http {
namespace server {
namespace ygi {
YgiRequestHandler::YgiRequestHandler(ygi_request_handler_t ygi_request_handler)
  : ygi_request_handler(ygi_request_handler) {
}

void YgiRequestHandler::handle(YO_NEW_REF Event& event) {
  CHECK_EQ(event.get_type_id(), HttpRequest::TYPE_ID);
  HttpRequest& http_request = static_cast<HttpRequest&>(event);

  YgiRequest ygi_request(http_request);
  ygi_request_handler(&ygi_request);
}
}
}
}
}
