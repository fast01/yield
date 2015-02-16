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

#ifndef _YIELD_HTTP_SERVER_YGI_YGI_REQUEST_HPP_
#define _YIELD_HTTP_SERVER_YGI_YGI_REQUEST_HPP_

#include "yield/http/server/http_server_request.hpp"

#include <ygi.h>

namespace yield {
namespace http {
namespace server {
namespace ygi {
class YgiRequest final : public ygi_request_t {
public:
  YgiRequest(::std::unique_ptr<HttpServerRequest> http_request);

public:
  // RFC 3875 meta-variables
  iovec AUTH_TYPE() const;
  size_t CONTENT_LENGTH() const;
  iovec CONTENT_TYPE() const;
  iovec DOCUMENT_ROOT() const;
  iovec GATEWAY_INTERFACE() const;
  iovec HTTP_(const char* field_name) const;
  iovec HTTP_REFERER() const;
  iovec HTTP_USER_AGENT() const;
  iovec PATH_INFO() const;
  iovec PATH_TRANSLATED() const;
  iovec QUERY_STRING() const;
  iovec REMOTE_ADDR() const;
  iovec REMOTE_HOST() const;
  iovec REMOTE_IDENT() const;
  iovec REMOTE_USER() const;
  iovec REQUEST_METHOD() const;
  iovec SCRIPT_NAME() const;
  iovec SERVER_NAME() const;
  uint16_t SERVER_PORT() const;
  iovec SERVER_PROTOCOL() const;
  iovec SERVER_SOFTWARE() const;

public:
  ygi_response_t* respond(uint16_t status_code);

private:
  static iovec static_AUTH_TYPE(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->AUTH_TYPE();
  }

  static size_t static_CONTENT_LENGTH(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->CONTENT_LENGTH();
  }

  static iovec static_CONTENT_TYPE(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->CONTENT_TYPE();
  }

  static iovec static_DOCUMENT_ROOT(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->DOCUMENT_ROOT();
  }

  static iovec static_GATEWAY_INTERFACE(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->GATEWAY_INTERFACE();
  }

  static iovec static_HTTP_(const ygi_request_t* this_, const char* field_name) {
    return static_cast<const YgiRequest*>(this_)->HTTP_(field_name);
  }

  static iovec static_HTTP_REFERER(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->HTTP_REFERER();
  }

  static iovec static_HTTP_USER_AGENT(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->HTTP_USER_AGENT();
  }

  static iovec static_PATH_INFO(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->PATH_INFO();
  }

  static iovec static_PATH_TRANSLATED(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->PATH_TRANSLATED();
  }

  static iovec static_QUERY_STRING(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->QUERY_STRING();
  }

  static iovec static_REMOTE_ADDR(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->REMOTE_ADDR();
  }

  static iovec static_REMOTE_HOST(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->REMOTE_HOST();
  }

  static iovec static_REMOTE_IDENT(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->REMOTE_IDENT();
  }

  static iovec static_REMOTE_USER(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->REMOTE_USER();
  }

  static iovec static_REQUEST_METHOD(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->REQUEST_METHOD();
  }

  static iovec static_SCRIPT_NAME(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->SCRIPT_NAME();
  }

  static iovec static_SERVER_NAME(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->SERVER_NAME();
  }

  static uint16_t static_SERVER_PORT(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->SERVER_PORT();
  }

  static iovec static_SERVER_PROTOCOL(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->SERVER_PROTOCOL();
  }

  static iovec static_SERVER_SOFTWARE(const ygi_request_t* this_) {
    return static_cast<const YgiRequest*>(this_)->SERVER_SOFTWARE();
  }

  static ygi_response_t*
  static_respond(
    ygi_request_t* this_,
    uint16_t status_code
  ) {
    return static_cast<YgiRequest*>(this_)->respond(status_code);
  }

  ygi_response_t* static_respond(uint16_t status_code);

private:
  ::std::unique_ptr<HttpServerRequest> http_request_;
  ::std::string remote_addr_;
  ::std::string remote_host_;
  iovec server_protocol_;
};
}
}
}
}

#endif
