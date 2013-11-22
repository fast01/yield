// yield/uri/uri.cpp

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

#include "yield/buffer.hpp"
#include "yield/exception.hpp"
#include "yield/uri/uri.hpp"
#include "yield/uri/uri_parser.hpp"

#include <sstream> // or std::ostringstream

namespace yield {
namespace uri {
Uri::Uri(
  Buffer& buffer,
  const iovec& fragment,
  const iovec& host,
  const iovec& path,
  uint16_t port,
  const iovec& query,
  const iovec& scheme,
  const iovec& userinfo
)
  : buffer(&buffer.inc_ref()),
    fragment(fragment),
    host(host),
    path(path),
    port(port),
    query(query),
    scheme(scheme),
    userinfo(userinfo) {
}

Uri::Uri(const char* uri) throw(Exception)
  : buffer(NULL) {
  init(uri, strlen(uri));
}

Uri::Uri(const string& uri) throw(Exception)
  : buffer(NULL) {
  init(uri.data(), uri.size());
}

Uri::Uri(const char* uri, size_t uri_len) throw(Exception)
  : buffer(NULL) {
  init(uri, uri_len);
}

Uri::Uri(Uri& other)
  : buffer(&other.buffer->inc_ref()),
    fragment(other.fragment),
    host(other.host),
    path(other.path),
    port(other.port),
    query(other.query),
    scheme(other.scheme),
    userinfo(other.userinfo)
{ }

Uri::Uri(const Uri& other)
  : buffer(&other.buffer->copy()),
    port(other.port) {
  char* old_base = static_cast<char*>(*other.buffer);
  char* new_base = static_cast<char*>(*this->buffer);
  rebase(old_base, other.fragment, new_base, fragment);
  rebase(old_base, other.host, new_base, host);
  rebase(old_base, other.path, new_base, path);
  rebase(old_base, other.query, new_base, query);
  rebase(old_base, other.scheme, new_base, scheme);
  rebase(old_base, other.userinfo, new_base, userinfo);
}

Uri::~Uri() {
  Buffer::dec_ref(*buffer);
}

void Uri::init(const char* uri, size_t uri_len) {
  buffer = &Buffer::copy(uri, uri_len);

  if (
    !UriParser(*buffer).parse(
      fragment,
      host,
      path,
      port,
      query,
      scheme,
      userinfo
    )
  ) {
    throw Exception("invalid Uri");
  }
}

string Uri::iovec_to_string(const iovec& iovec_) {
  return string(static_cast<char*>(iovec_.iov_base), iovec_.iov_len);
}

std::ostream& operator<<(std::ostream& os, const Uri& uri) {
  if (uri.has_scheme()) {
    os.write(static_cast<char*>(uri.scheme.iov_base), uri.scheme.iov_len);
    os.write("://", 3);
  }

  if (uri.has_userinfo()) {
    os.write(static_cast<char*>(uri.userinfo.iov_base), uri.userinfo.iov_len);
  }

  if (uri.has_host()) {
    os.write(static_cast<char*>(uri.host.iov_base), uri.host.iov_len);
  }

  if (uri.has_port()) {
    os << ":" << uri.port;
  }

  if (uri.has_path()) {
    os.write(static_cast<char*>(uri.path.iov_base), uri.path.iov_len);
  }

  if (uri.has_query()) {
    os.write("?", 1);
    os.write(static_cast<char*>(uri.query.iov_base), uri.query.iov_len);
  }

  if (uri.has_fragment()) {
    os.write("#", 1);
    os.write(static_cast<char*>(uri.fragment.iov_base), uri.fragment.iov_len);
  }

  return os;
}

Uri::operator string() const {
  std::ostringstream uri;
  uri << *this;
  return uri.str();
}

bool Uri::operator==(const Uri& other) const {
  return
    fragment.iov_len == other.fragment.iov_len
    &&
    memcmp(fragment.iov_base, other.fragment.iov_base, fragment.iov_len) == 0
    &&
    host.iov_len == other.host.iov_len
    &&
    memcmp(host.iov_base, other.host.iov_base, host.iov_len) == 0
    &&
    path.iov_len == other.path.iov_len
    &&
    memcmp(path.iov_base, other.path.iov_base, path.iov_len) == 0
    &&
    query.iov_len == other.query.iov_len
    &&
    port == other.port
    &&
    memcmp(query.iov_base, other.query.iov_base, query.iov_len) == 0
    &&
    scheme.iov_len == other.scheme.iov_len
    &&
    memcmp(scheme.iov_base, other.scheme.iov_base, scheme.iov_len) == 0
    &&
    userinfo.iov_len == other.userinfo.iov_len
    &&
    memcmp(userinfo.iov_base, other.userinfo.iov_base, userinfo.iov_len) == 0;
}

bool Uri::operator<(const Uri& other) const {
  size_t cmp_max = \
    this->buffer->size() < other.buffer->size() ?
      this->buffer->size() :
      other.buffer->size();
  return memcmp(*this->buffer, *other.buffer, cmp_max) < 0;
}

Uri Uri::operator+(const char* s) const {
  std::ostringstream uri;
  uri << *this;
  uri << s;
  return Uri(uri.str());
}

Uri Uri::operator+(const string& s) const {
  std::ostringstream uri;
  uri << *this;
  uri << s;
  return Uri(uri.str());
}

void
Uri::rebase(
  char* old_base,
  const iovec& old_iov,
  char* new_base,
  iovec& new_iov
) {
  if (old_iov.iov_base != NULL) {
    if (old_iov.iov_base >= old_base) {
      new_iov.iov_base
      = new_base + (static_cast<char*>(old_iov.iov_base) - old_base);
      new_iov.iov_len = old_iov.iov_len;
    } else { // assume old_iov points to a literal
      new_iov = old_iov;
    }
  } else {
    memset(&new_iov, 0, sizeof(new_iov));
  }
}
}
}
