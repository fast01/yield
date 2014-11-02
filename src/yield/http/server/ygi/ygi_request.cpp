// yield/http/server/ygi/ygi_request.cpp

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

#include "yield/http/server/http_connection.hpp"
#include "yield/http/server/ygi/ygi_request.hpp"

#include <yield/logging.hpp>

namespace yield {
namespace http {
namespace server {
namespace ygi {
using ::yield::sockets::SocketAddress;

YgiRequest::YgiRequest(HttpRequest& http_request)
  : http_request_(http_request) {
  ygi_request_t::AUTH_TYPE = static_AUTH_TYPE;
  ygi_request_t::CONTENT_LENGTH = static_CONTENT_LENGTH;
  ygi_request_t::CONTENT_TYPE = static_CONTENT_TYPE;
  ygi_request_t::DOCUMENT_ROOT = static_DOCUMENT_ROOT;
  ygi_request_t::GATEWAY_INTERFACE = static_GATEWAY_INTERFACE;
  ygi_request_t::HTTP_ = static_HTTP_;
  ygi_request_t::HTTP_REFERER = static_HTTP_REFERER;
  ygi_request_t::HTTP_USER_AGENT = static_HTTP_USER_AGENT;
  ygi_request_t::PATH_INFO = static_PATH_INFO;
  ygi_request_t::PATH_TRANSLATED = static_PATH_TRANSLATED;
  ygi_request_t::QUERY_STRING = static_QUERY_STRING;
  ygi_request_t::REMOTE_ADDR = static_REMOTE_ADDR;
  ygi_request_t::REMOTE_HOST = static_REMOTE_HOST;
  ygi_request_t::REMOTE_IDENT = static_REMOTE_IDENT;
  ygi_request_t::REMOTE_USER = static_REMOTE_USER;
  ygi_request_t::REQUEST_METHOD = static_REQUEST_METHOD;
  ygi_request_t::respond = static_respond;
  ygi_request_t::SCRIPT_NAME = static_SCRIPT_NAME;
  ygi_request_t::SERVER_NAME = static_SERVER_NAME;
  ygi_request_t::SERVER_PORT = static_SERVER_PORT;
  ygi_request_t::SERVER_PROTOCOL = static_SERVER_PROTOCOL;
  ygi_request_t::SERVER_SOFTWARE = static_SERVER_SOFTWARE;

  http_request_.get_connection().get_peername().getnameinfo(remote_addr_, NULL, SocketAddress::GETNAMEINFO_FLAG_NUMERICHOST | SocketAddress::GETNAMEINFO_FLAG_NUMERICSERV);
  http_request_.get_connection().get_peername().getnameinfo(remote_host_, NULL, SocketAddress::GETNAMEINFO_FLAG_NAMEREQD | SocketAddress::GETNAMEINFO_FLAG_NUMERICSERV);

  if (http_request_.get_http_version() == 0) {
    server_protocol_.iov_base = "HTTP/1.0";
  } else if (http_request.get_http_version() == 1) {
    server_protocol_.iov_base = "HTTP/1.1";
  } else {
    CHECK(false);
  }
  server_protocol_.iov_len = 8;
}

iovec YgiRequest::AUTH_TYPE() const {
  iovec auth_type = { 0, 0 };
  return auth_type;
}

size_t YgiRequest::CONTENT_LENGTH() const {
  return http_request_.get_content_length();
}

iovec YgiRequest::CONTENT_TYPE() const {
  iovec field_value = { 0, 0 };
  http_request_.get_field("Content-Type", field_value);
  return field_value;
}

iovec YgiRequest::DOCUMENT_ROOT() const {
  iovec document_root = { 0, 0 };
  return document_root;
}

iovec YgiRequest::GATEWAY_INTERFACE() const {
  iovec gateway_interface;
  gateway_interface.iov_base = "CGI/1.1";
  gateway_interface.iov_len = 7;
  return gateway_interface;
}

iovec YgiRequest::HTTP_(const char* field_name) const {
  iovec field_value = { 0, 0 };
  http_request_.get_field(field_name, field_value);
  return field_value;
}

iovec YgiRequest::HTTP_REFERER() const {
  iovec field_value = { 0, 0 };
  http_request_.get_field("Referer", field_value);
  return field_value;
}

iovec YgiRequest::HTTP_USER_AGENT() const {
  iovec field_value = { 0, 0 };
  http_request_.get_field("User-Agent", field_value);
  return field_value;
}

iovec YgiRequest::PATH_INFO() const {
  iovec path_info = { 0, 0 };
  http_request_.get_uri().get_path(path_info);
  return path_info;
}

iovec YgiRequest::PATH_TRANSLATED() const {
  return PATH_INFO();
}

iovec YgiRequest::QUERY_STRING() const {
  iovec query_string = { 0, 0 };
  http_request_.get_uri().get_query(query_string);
  return query_string;
}

iovec YgiRequest::REMOTE_ADDR() const {
  iovec remote_addr;
  remote_addr.iov_base = const_cast<char*>(remote_addr_.c_str());
  remote_addr.iov_len = remote_addr_.size();
  return remote_addr;
}

iovec YgiRequest::REMOTE_HOST() const {
  iovec remote_host;
  remote_host.iov_base = const_cast<char*>(remote_host_.c_str());
  remote_host.iov_len = remote_host_.size();
  return remote_host;
}

iovec YgiRequest::REMOTE_IDENT() const {
  iovec remote_ident = { 0, 0 };
  return remote_ident;
}

iovec YgiRequest::REMOTE_USER() const {
  iovec remote_user = { 0, 0 };
  return remote_user;
}

iovec YgiRequest::REQUEST_METHOD() const  {
  iovec request_method;
  request_method.iov_base
  = const_cast<char*>(http_request_.get_method().get_name());
  request_method.iov_len = http_request_.get_method().get_name_len();
  return request_method;
}

ygi_response_t* YgiRequest::respond(uint16_t status_code) {
  http_request_.respond(status_code);
  return NULL;
}

iovec YgiRequest::SCRIPT_NAME() const  {
  iovec remote_host = { 0, 0 };
  return remote_host;
}

iovec YgiRequest::SERVER_NAME() const {
  iovec field_value = { 0, 0 };
  CHECK(http_request_.get_field("Host", field_value));
  return field_value;
}

uint16_t YgiRequest::SERVER_PORT() const {
  if (http_request_.get_uri().has_port()) {
    return http_request_.get_uri().get_port();
  } else {
    return 80;
  }
}

iovec YgiRequest::SERVER_PROTOCOL() const {
  return server_protocol_;
}

iovec YgiRequest::SERVER_SOFTWARE() const {
  iovec server_software;
  server_software.iov_base = "Yield";
  server_software.iov_len = 5;
  return server_software;
}
}
}
}
}
