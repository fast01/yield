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
#include "yield/logging.hpp"
#include "yield/http/http_response_parser.hpp"

#include <stdlib.h> // For atof and atoi

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4702)
#else
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

namespace yield {
namespace http {
void HttpResponseParser::parse(ParseCallbacks& callbacks) {
  if (p < eof) {
    ps = p;

    uint8_t http_version;
    uint16_t status_code;
    if (parse_status_line(http_version, status_code)) {
      uint16_t fields_offset;
      size_t content_length;
      if (parse_fields(fields_offset, content_length)) {
        ::std::shared_ptr<Buffer> body;
        if (parse_body(content_length, body)) {
          callbacks.handle_http_response(body, fields_offset, buffer_, http_version, status_code);
        } else {
          ::std::shared_ptr<Buffer> next_buffer
            = Buffer::copy(
                Buffer::getpagesize(),
                p - ps + content_length,
                ps,
                eof - ps
              );
          ps = p;
          callbacks.read(next_buffer);
          return;
        }
      }
    } else {
      parse_body_chunks(callbacks);
      return;
    }

    if (p == eof) { // EOF parsing
      ::std::shared_ptr<Buffer> next_buffer
        = Buffer::copy(
            Buffer::getpagesize(),
            eof - ps + Buffer::getpagesize(),
            ps,
            eof - ps
          );
      p = ps;
      callbacks.read(next_buffer);
    } else { // Error parsing
      callbacks.handle_http_response(NULL, http_version, 400);
    }
  } else { // p == eof
    callbacks.read(::std::make_shared<Buffer>(Buffer::getpagesize(), Buffer::getpagesize()));
  }
}

bool
HttpResponseParser::parse_status_line(
  uint8_t& http_version,
  uint16_t& status_code
) {
  int cs;

  %%{
    machine status_line_parser;
    alphtype unsigned char;

    include rfc2616 "rfc2616.rl";

    # RFC 2616 6.1.1
    status_code = digit+
                  >{ status_code = static_cast<uint16_t>(atoi(p)); };

    reason_phrase = (alpha | ' ')+;

    # RFC 2616 6.1
    status_line = http_version ' '+ status_code ' '+ reason_phrase crlf;

    main := status_line
            @{ fbreak; }
            $err{ return false; };

    write data;
    write init;
    write exec noend;
  }%%

  return cs != status_line_parser_error;
}
}
}

#ifdef _WIN32
#pragma warning(pop)
#else
#pragma GCC diagnostic warning "-Wold-style-cast"
#pragma GCC diagnostic warning "-Wunused-variable"
#endif
//
