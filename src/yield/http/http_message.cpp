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
#include "yield/date_time.hpp"
#include "yield/logging.hpp"
#include "yield/fs/file.hpp"
#include "yield/http/http_message.hpp"
#include "yield/http/http_message_parser.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_response.hpp"

#ifdef _WIN32
#include <Windows.h> // For SYSTEMTIME
#pragma warning(push)
#pragma warning(disable:4702)
#else
#include <stdio.h> // For snprintf
#include <stdlib.h> // For atoi
#endif

namespace yield {
namespace http {
using ::std::make_shared;
using ::std::shared_ptr;
using ::yield::fs::File;

template <class HttpMessageType>
HttpMessage<HttpMessageType>::HttpMessage(
  shared_ptr<Buffer> body_buffer,
  shared_ptr<File> body_file,
  uint8_t http_version
) : body_buffer_(body_buffer),
    body_file_(body_file),
  header_(make_shared<Buffer>(Buffer::getpagesize(), Buffer::getpagesize())),
  http_version_(http_version) {
  if (body_buffer != NULL) {
    CHECK_EQ(body_file, NULL);
  }
  fields_offset_ = 0;
}

template <class HttpMessageType>
HttpMessage<HttpMessageType>::
HttpMessage(
  shared_ptr<Buffer> body_buffer,
  shared_ptr<File> body_file,
  uint16_t fields_offset,
  shared_ptr<Buffer> header,
  uint8_t http_version
) : body_buffer_(body_buffer),
    body_file_(body_file),
  fields_offset_(fields_offset),
  header_(header),
  http_version_(http_version) {
  if (body_buffer) {
    CHECK(!body_file);
  }
}

template <class HttpMessageType>
void HttpMessage<HttpMessageType>::finalize() {
  header_->put("\r\n", 2);
}

template <class HttpMessageType>
size_t HttpMessage<HttpMessageType>::get_content_length() const {
  size_t content_length = 0;
  HttpMessageParser::parse_content_length_field(
    static_cast<const char*>(*header_) + fields_offset_,
    static_cast<const char*>(*header_) + header_->size(),
    content_length
  );
  return content_length;
}

template <class HttpMessageType>
DateTime HttpMessage<HttpMessageType>::get_date_field(const char* name) const {
  iovec value;
  if (get_field(name, value)) {
    return HttpMessageParser::parse_date(value);
  } else {
    return DateTime::INVALID_DATE_TIME;
  }
}

template <class HttpMessageType>
bool
HttpMessage<HttpMessageType>::get_field(
  const char* name,
  size_t name_len,
  iovec& value
) const {
  iovec name_iov;
  name_iov.iov_base = const_cast<char*>(name);
  name_iov.iov_len = name_len;
  return HttpMessageParser::parse_field(
           static_cast<const char*>(*header_) + fields_offset_,
           static_cast<const char*>(*header_) + header_->size(),
           name_iov,
           value
         );
}

template <class HttpMessageType>
void
HttpMessage<HttpMessageType>::get_fields(
  ::std::vector< std::pair<iovec, iovec> >& fields
) const {
  return HttpMessageParser::parse_fields(
           static_cast<const char*>(*header_) + fields_offset_,
           static_cast<const char*>(*header_) + header_->size(),
           fields
         );
}

template <class HttpMessageType>
void HttpMessage<HttpMessageType>::set_body(shared_ptr<Buffer> body_buffer) {
  body_buffer_.swap(body_buffer);
  body_file_.reset();
}

template <class HttpMessageType>
void HttpMessage<HttpMessageType>::set_body(shared_ptr<File> body_file) {
  body_buffer_.reset();
  body_file_.swap(body_file);
}

template <class HttpMessageType>
HttpMessageType&
HttpMessage<HttpMessageType>::
set_field(
  const char* name,
  size_t name_len,
  const DateTime& value
) {
  static const char* months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };

  static const char* week_days[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };

  char date[30];
  int date_len;

#ifdef _WIN32
  SYSTEMTIME utc_system_time = value.as_utc_SYSTEMTIME();

  date_len
  = _snprintf_s(
      date,
      30,
      _TRUNCATE,
      "%s, %02d %s %04d %02d:%02d:%02d GMT",
      week_days[utc_system_time.wDayOfWeek],
      utc_system_time.wDay,
      months[utc_system_time.wMonth - 1],
      utc_system_time.wYear,
      utc_system_time.wHour,
      utc_system_time.wMinute,
      utc_system_time.wSecond
    );
#else
  tm utc_tm = value.as_utc_tm();

  date_len
  = snprintf(
      date,
      30,
      "%s, %02d %s %04d %02d:%02d:%02d GMT",
      week_days[utc_tm.tm_wday],
      utc_tm.tm_mday,
      months[utc_tm.tm_mon],
      utc_tm.tm_year + 1900,
      utc_tm.tm_hour,
      utc_tm.tm_min,
      utc_tm.tm_sec
    );
#endif

  return set_field(name, name_len, date, date_len);
}

template <class HttpMessageType>
HttpMessageType&
HttpMessage<HttpMessageType>::
set_field(
  const char* name,
  size_t name_len,
  size_t value
) {
  char value_str[64];
  int value_str_len;

#ifdef _WIN32
  value_str_len = sprintf_s(value_str, 64, "%u", value);
#else
  value_str_len = snprintf(value_str, 64, "%zu", value);
#endif

  return set_field(name, name_len, value_str, value_str_len);
}

template <class HttpMessageType>
HttpMessageType&
HttpMessage<HttpMessageType>::
set_field(
  const char* name,
  size_t name_len,
  const void* value,
  size_t value_len
) {
  CHECK_GT(fields_offset_, 0);
  CHECK_GT(name_len, 0);
  CHECK_GT(value_len, 0);

  header_->put(name, name_len);
  header_->put(": ", 2);
  header_->put(value, value_len);
  header_->put("\r\n");

  return static_cast<HttpMessageType&>(*this);
}

template class HttpMessage<HttpRequest>;
template class HttpMessage<HttpResponse>;
}
}

#ifdef _WIN32
#pragma warning(pop)
#endif
//
