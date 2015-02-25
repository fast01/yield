
/* #line 1 "http_message_parser.rl" */
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
#include "yield/http/http_message_body_chunk.hpp"
#include "yield/http/http_message_parser.hpp"
#include "yield/http/http_request.hpp"

#include <stdlib.h> // For strtol

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4702)
#else
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

namespace yield {
namespace http {
HttpMessageParser::HttpMessageParser(::std::shared_ptr<Buffer> buffer)
  : buffer_(buffer) {
  CHECK(!buffer->empty());

  ps = p = *buffer_;
  eof = ps + buffer_->size();
}

HttpMessageParser::HttpMessageParser(const ::std::string& buffer)
  : buffer_(Buffer::copy(buffer)) {
  CHECK(!buffer.empty());

  ps = p = *buffer_;
  eof = ps + buffer_->size();
}

bool
HttpMessageParser::parse_body(
  size_t content_length,
  ::std::shared_ptr<Buffer>& out_body
) {
  if (
    content_length == 0
    ||
    content_length == HttpRequest::CONTENT_LENGTH_CHUNKED
 ) {
    return true;
  } else if (static_cast<size_t>(eof - p) >= content_length) {
    out_body = Buffer::copy(p, content_length);
    p += content_length;
    return true;
  } else
    return false;
}

void HttpMessageParser::parse_body_chunks(ParseCallbacks& callbacks) {
  const char* chunk_data_p = NULL;
  size_t chunk_size = 0;
  const char* chunk_size_p = NULL;
  int cs;
  iovec field_name = {0, 0}, field_value = {0, 0};
  size_t seen_chunk_size = 0;

  ps = p;

  
/* #line 92 "http_message_parser.cpp" */
static const char _chunk_parser_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	8, 2, 0, 4, 2, 0, 6, 2, 
	2, 3, 2, 6, 4, 2, 7, 5, 
	3, 0, 6, 4
};

static const char _chunk_parser_cond_offsets[] = {
	0, 0, 0, 0, 0, 1, 1, 1, 
	1, 2, 3, 4, 5, 5, 5, 5, 
	5, 5, 5, 5, 5, 5, 5, 5, 
	5, 5, 5, 5, 5, 5, 5, 5, 
	5, 5, 5, 5, 5, 5, 5, 5, 
	5, 5, 5, 5, 5, 6, 7, 7, 
	8, 9, 10, 11, 11, 12, 13, 14, 
	15, 16, 17, 18, 19, 20, 21, 21, 
	22, 23, 24, 25, 26, 27, 28, 29, 
	30, 31, 32, 33, 34, 34, 35, 36, 
	37, 38, 39, 40, 40, 41, 42, 42, 
	43
};

static const char _chunk_parser_cond_lengths[] = {
	0, 0, 0, 0, 1, 0, 0, 0, 
	1, 1, 1, 1, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 1, 1, 0, 1, 
	1, 1, 1, 0, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 0, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 0, 1, 1, 1, 
	1, 1, 1, 0, 1, 1, 0, 1, 
	1
};

static const short _chunk_parser_cond_keys[] = {
	0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u, 
	0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u, 
	0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u, 
	0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u, 
	0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u, 
	0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u, 
	0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u, 
	0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u, 
	0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u, 
	0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u, 
	0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u, 
	0
};

static const char _chunk_parser_cond_spaces[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0
};

static const short _chunk_parser_key_offsets[] = {
	0, 0, 7, 15, 16, 54, 55, 63, 
	64, 68, 72, 78, 102, 103, 118, 123, 
	124, 139, 143, 166, 189, 203, 220, 235, 
	251, 272, 277, 279, 281, 288, 307, 329, 
	347, 361, 378, 393, 409, 430, 435, 437, 
	439, 446, 465, 487, 505, 545, 558, 559, 
	565, 621, 627, 683, 684, 708, 714, 752, 
	794, 834, 874, 923, 938, 944, 954, 960, 
	977, 994, 1041, 1092, 1137, 1175, 1217, 1257, 
	1297, 1346, 1361, 1367, 1377, 1383, 1400, 1417, 
	1464, 1515, 1560, 1571, 1578, 1602, 1656, 1678, 
	1732
};

static const short _chunk_parser_trans_keys[] = {
	48u, 49u, 57u, 65u, 70u, 97u, 102u, 13u, 
	59u, 48u, 57u, 65u, 70u, 97u, 102u, 10u, 
	269u, 380u, 382u, 525u, 556u, 559u, 635u, 637u, 
	289u, 294u, 298u, 299u, 301u, 302u, 304u, 313u, 
	321u, 346u, 350u, 378u, 512u, 544u, 545u, 550u, 
	551u, 553u, 554u, 569u, 570u, 576u, 577u, 602u, 
	603u, 605u, 606u, 638u, 639u, 767u, 10u, 13u, 
	59u, 48u, 57u, 65u, 70u, 97u, 102u, 10u, 
	269u, 525u, 512u, 767u, 269u, 525u, 512u, 767u, 
	266u, 269u, 522u, 525u, 512u, 767u, 269u, 315u, 
	525u, 571u, 304u, 313u, 321u, 326u, 353u, 358u, 
	512u, 559u, 560u, 569u, 570u, 576u, 577u, 582u, 
	583u, 608u, 609u, 614u, 615u, 767u, 10u, 58u, 
	124u, 126u, 33u, 38u, 42u, 43u, 45u, 46u, 
	48u, 57u, 65u, 90u, 94u, 122u, 13u, 32u, 
	127u, 0u, 31u, 10u, 13u, 124u, 126u, 33u, 
	38u, 42u, 43u, 45u, 46u, 48u, 57u, 65u, 
	90u, 94u, 122u, 13u, 127u, 0u, 31u, 13u, 
	58u, 59u, 124u, 126u, 33u, 38u, 42u, 43u, 
	45u, 46u, 48u, 57u, 65u, 70u, 71u, 90u, 
	94u, 96u, 97u, 102u, 103u, 122u, 13u, 58u, 
	59u, 124u, 126u, 33u, 38u, 42u, 43u, 45u, 
	46u, 48u, 57u, 65u, 70u, 71u, 90u, 94u, 
	96u, 97u, 102u, 103u, 122u, 124u, 126u, 33u, 
	38u, 42u, 43u, 45u, 46u, 48u, 57u, 65u, 
	90u, 94u, 122u, 13u, 59u, 61u, 124u, 126u, 
	33u, 38u, 42u, 43u, 45u, 46u, 48u, 57u, 
	65u, 90u, 94u, 122u, 34u, 124u, 126u, 33u, 
	38u, 42u, 43u, 45u, 46u, 48u, 57u, 65u, 
	90u, 94u, 122u, 13u, 59u, 124u, 126u, 33u, 
	38u, 42u, 43u, 45u, 46u, 48u, 57u, 65u, 
	90u, 94u, 122u, 13u, 34u, 59u, 92u, 124u, 
	126u, 127u, 0u, 31u, 33u, 38u, 42u, 43u, 
	45u, 46u, 48u, 57u, 65u, 90u, 94u, 122u, 
	34u, 92u, 127u, 0u, 31u, 13u, 59u, 34u, 
	92u, 13u, 34u, 59u, 92u, 127u, 0u, 31u, 
	34u, 92u, 124u, 126u, 127u, 0u, 31u, 33u, 
	38u, 42u, 43u, 45u, 46u, 48u, 57u, 65u, 
	90u, 94u, 122u, 13u, 34u, 59u, 61u, 92u, 
	124u, 126u, 127u, 0u, 31u, 33u, 38u, 42u, 
	43u, 45u, 46u, 48u, 57u, 65u, 90u, 94u, 
	122u, 92u, 124u, 126u, 127u, 0u, 31u, 33u, 
	38u, 42u, 43u, 45u, 46u, 48u, 57u, 65u, 
	90u, 94u, 122u, 124u, 126u, 33u, 38u, 42u, 
	43u, 45u, 46u, 48u, 57u, 65u, 90u, 94u, 
	122u, 13u, 59u, 61u, 124u, 126u, 33u, 38u, 
	42u, 43u, 45u, 46u, 48u, 57u, 65u, 90u, 
	94u, 122u, 34u, 124u, 126u, 33u, 38u, 42u, 
	43u, 45u, 46u, 48u, 57u, 65u, 90u, 94u, 
	122u, 13u, 59u, 124u, 126u, 33u, 38u, 42u, 
	43u, 45u, 46u, 48u, 57u, 65u, 90u, 94u, 
	122u, 13u, 34u, 59u, 92u, 124u, 126u, 127u, 
	0u, 31u, 33u, 38u, 42u, 43u, 45u, 46u, 
	48u, 57u, 65u, 90u, 94u, 122u, 34u, 92u, 
	127u, 0u, 31u, 13u, 59u, 34u, 92u, 13u, 
	34u, 59u, 92u, 127u, 0u, 31u, 34u, 92u, 
	124u, 126u, 127u, 0u, 31u, 33u, 38u, 42u, 
	43u, 45u, 46u, 48u, 57u, 65u, 90u, 94u, 
	122u, 13u, 34u, 59u, 61u, 92u, 124u, 126u, 
	127u, 0u, 31u, 33u, 38u, 42u, 43u, 45u, 
	46u, 48u, 57u, 65u, 90u, 94u, 122u, 92u, 
	124u, 126u, 127u, 0u, 31u, 33u, 38u, 42u, 
	43u, 45u, 46u, 48u, 57u, 65u, 90u, 94u, 
	122u, 269u, 314u, 380u, 382u, 525u, 556u, 559u, 
	570u, 635u, 637u, 289u, 294u, 298u, 299u, 301u, 
	302u, 304u, 313u, 321u, 346u, 350u, 378u, 512u, 
	544u, 545u, 550u, 551u, 553u, 554u, 569u, 571u, 
	576u, 577u, 602u, 603u, 605u, 606u, 638u, 639u, 
	767u, 269u, 288u, 525u, 544u, 639u, 289u, 382u, 
	384u, 511u, 512u, 543u, 545u, 767u, 10u, 266u, 
	269u, 522u, 525u, 512u, 767u, 269u, 314u, 315u, 
	380u, 382u, 525u, 556u, 559u, 570u, 571u, 635u, 
	637u, 289u, 294u, 298u, 299u, 301u, 302u, 304u, 
	313u, 321u, 326u, 327u, 346u, 350u, 352u, 353u, 
	358u, 359u, 378u, 512u, 544u, 545u, 550u, 551u, 
	553u, 554u, 558u, 560u, 569u, 572u, 576u, 577u, 
	582u, 583u, 602u, 603u, 605u, 606u, 608u, 609u, 
	614u, 615u, 638u, 639u, 767u, 266u, 269u, 522u, 
	525u, 512u, 767u, 269u, 314u, 315u, 380u, 382u, 
	525u, 556u, 559u, 570u, 571u, 635u, 637u, 289u, 
	294u, 298u, 299u, 301u, 302u, 304u, 313u, 321u, 
	326u, 327u, 346u, 350u, 352u, 353u, 358u, 359u, 
	378u, 512u, 544u, 545u, 550u, 551u, 553u, 554u, 
	558u, 560u, 569u, 572u, 576u, 577u, 582u, 583u, 
	602u, 603u, 605u, 606u, 608u, 609u, 614u, 615u, 
	638u, 639u, 767u, 10u, 269u, 315u, 525u, 571u, 
	304u, 313u, 321u, 326u, 353u, 358u, 512u, 559u, 
	560u, 569u, 570u, 576u, 577u, 582u, 583u, 608u, 
	609u, 614u, 615u, 767u, 266u, 269u, 522u, 525u, 
	512u, 767u, 269u, 380u, 382u, 525u, 556u, 559u, 
	635u, 637u, 289u, 294u, 298u, 299u, 301u, 302u, 
	304u, 313u, 321u, 346u, 350u, 378u, 512u, 544u, 
	545u, 550u, 551u, 553u, 554u, 569u, 570u, 576u, 
	577u, 602u, 603u, 605u, 606u, 638u, 639u, 767u, 
	269u, 315u, 317u, 380u, 382u, 525u, 556u, 559u, 
	571u, 573u, 635u, 637u, 289u, 294u, 298u, 299u, 
	301u, 302u, 304u, 313u, 321u, 346u, 350u, 378u, 
	512u, 544u, 545u, 550u, 551u, 553u, 554u, 569u, 
	570u, 576u, 577u, 602u, 603u, 605u, 606u, 638u, 
	639u, 767u, 269u, 290u, 380u, 382u, 525u, 546u, 
	556u, 559u, 635u, 637u, 289u, 294u, 298u, 299u, 
	301u, 302u, 304u, 313u, 321u, 346u, 350u, 378u, 
	512u, 544u, 545u, 550u, 551u, 553u, 554u, 569u, 
	570u, 576u, 577u, 602u, 603u, 605u, 606u, 638u, 
	639u, 767u, 269u, 315u, 380u, 382u, 525u, 556u, 
	559u, 571u, 635u, 637u, 289u, 294u, 298u, 299u, 
	301u, 302u, 304u, 313u, 321u, 346u, 350u, 378u, 
	512u, 544u, 545u, 550u, 551u, 553u, 554u, 569u, 
	570u, 576u, 577u, 602u, 603u, 605u, 606u, 638u, 
	639u, 767u, 269u, 288u, 290u, 300u, 303u, 315u, 
	347u, 348u, 349u, 379u, 381u, 525u, 544u, 546u, 
	556u, 559u, 571u, 603u, 604u, 605u, 635u, 637u, 
	639u, 289u, 294u, 295u, 297u, 298u, 313u, 314u, 
	320u, 321u, 382u, 384u, 511u, 512u, 543u, 545u, 
	550u, 551u, 553u, 554u, 569u, 570u, 576u, 577u, 
	638u, 640u, 767u, 269u, 290u, 348u, 525u, 546u, 
	604u, 639u, 288u, 382u, 384u, 511u, 512u, 543u, 
	544u, 767u, 269u, 315u, 525u, 571u, 512u, 767u, 
	269u, 290u, 348u, 525u, 546u, 604u, 256u, 511u, 
	512u, 767u, 10u, 34u, 92u, 127u, 0u, 31u, 
	266u, 269u, 290u, 348u, 522u, 525u, 546u, 604u, 
	639u, 288u, 382u, 384u, 511u, 512u, 543u, 544u, 
	767u, 269u, 290u, 315u, 348u, 525u, 546u, 571u, 
	604u, 639u, 288u, 382u, 384u, 511u, 512u, 543u, 
	544u, 767u, 269u, 288u, 290u, 300u, 303u, 347u, 
	348u, 349u, 379u, 381u, 525u, 544u, 546u, 556u, 
	559u, 603u, 604u, 605u, 635u, 637u, 639u, 289u, 
	294u, 295u, 297u, 298u, 313u, 314u, 320u, 321u, 
	382u, 384u, 511u, 512u, 543u, 545u, 550u, 551u, 
	553u, 554u, 569u, 570u, 576u, 577u, 638u, 640u, 
	767u, 269u, 288u, 290u, 300u, 303u, 315u, 317u, 
	347u, 348u, 349u, 379u, 381u, 525u, 544u, 546u, 
	556u, 559u, 571u, 573u, 603u, 604u, 605u, 635u, 
	637u, 639u, 289u, 294u, 295u, 297u, 298u, 313u, 
	314u, 320u, 321u, 382u, 384u, 511u, 512u, 543u, 
	545u, 550u, 551u, 553u, 554u, 569u, 570u, 576u, 
	577u, 638u, 640u, 767u, 269u, 288u, 300u, 303u, 
	347u, 348u, 349u, 379u, 381u, 525u, 544u, 556u, 
	559u, 603u, 604u, 605u, 635u, 637u, 639u, 289u, 
	294u, 295u, 297u, 298u, 313u, 314u, 320u, 321u, 
	382u, 384u, 511u, 512u, 543u, 545u, 550u, 551u, 
	553u, 554u, 569u, 570u, 576u, 577u, 638u, 640u, 
	767u, 269u, 380u, 382u, 525u, 556u, 559u, 635u, 
	637u, 289u, 294u, 298u, 299u, 301u, 302u, 304u, 
	313u, 321u, 346u, 350u, 378u, 512u, 544u, 545u, 
	550u, 551u, 553u, 554u, 569u, 570u, 576u, 577u, 
	602u, 603u, 605u, 606u, 638u, 639u, 767u, 269u, 
	315u, 317u, 380u, 382u, 525u, 556u, 559u, 571u, 
	573u, 635u, 637u, 289u, 294u, 298u, 299u, 301u, 
	302u, 304u, 313u, 321u, 346u, 350u, 378u, 512u, 
	544u, 545u, 550u, 551u, 553u, 554u, 569u, 570u, 
	576u, 577u, 602u, 603u, 605u, 606u, 638u, 639u, 
	767u, 269u, 290u, 380u, 382u, 525u, 546u, 556u, 
	559u, 635u, 637u, 289u, 294u, 298u, 299u, 301u, 
	302u, 304u, 313u, 321u, 346u, 350u, 378u, 512u, 
	544u, 545u, 550u, 551u, 553u, 554u, 569u, 570u, 
	576u, 577u, 602u, 603u, 605u, 606u, 638u, 639u, 
	767u, 269u, 315u, 380u, 382u, 525u, 556u, 559u, 
	571u, 635u, 637u, 289u, 294u, 298u, 299u, 301u, 
	302u, 304u, 313u, 321u, 346u, 350u, 378u, 512u, 
	544u, 545u, 550u, 551u, 553u, 554u, 569u, 570u, 
	576u, 577u, 602u, 603u, 605u, 606u, 638u, 639u, 
	767u, 269u, 288u, 290u, 300u, 303u, 315u, 347u, 
	348u, 349u, 379u, 381u, 525u, 544u, 546u, 556u, 
	559u, 571u, 603u, 604u, 605u, 635u, 637u, 639u, 
	289u, 294u, 295u, 297u, 298u, 313u, 314u, 320u, 
	321u, 382u, 384u, 511u, 512u, 543u, 545u, 550u, 
	551u, 553u, 554u, 569u, 570u, 576u, 577u, 638u, 
	640u, 767u, 269u, 290u, 348u, 525u, 546u, 604u, 
	639u, 288u, 382u, 384u, 511u, 512u, 543u, 544u, 
	767u, 269u, 315u, 525u, 571u, 512u, 767u, 269u, 
	290u, 348u, 525u, 546u, 604u, 256u, 511u, 512u, 
	767u, 10u, 34u, 92u, 127u, 0u, 31u, 266u, 
	269u, 290u, 348u, 522u, 525u, 546u, 604u, 639u, 
	288u, 382u, 384u, 511u, 512u, 543u, 544u, 767u, 
	269u, 290u, 315u, 348u, 525u, 546u, 571u, 604u, 
	639u, 288u, 382u, 384u, 511u, 512u, 543u, 544u, 
	767u, 269u, 288u, 290u, 300u, 303u, 347u, 348u, 
	349u, 379u, 381u, 525u, 544u, 546u, 556u, 559u, 
	603u, 604u, 605u, 635u, 637u, 639u, 289u, 294u, 
	295u, 297u, 298u, 313u, 314u, 320u, 321u, 382u, 
	384u, 511u, 512u, 543u, 545u, 550u, 551u, 553u, 
	554u, 569u, 570u, 576u, 577u, 638u, 640u, 767u, 
	269u, 288u, 290u, 300u, 303u, 315u, 317u, 347u, 
	348u, 349u, 379u, 381u, 525u, 544u, 546u, 556u, 
	559u, 571u, 573u, 603u, 604u, 605u, 635u, 637u, 
	639u, 289u, 294u, 295u, 297u, 298u, 313u, 314u, 
	320u, 321u, 382u, 384u, 511u, 512u, 543u, 545u, 
	550u, 551u, 553u, 554u, 569u, 570u, 576u, 577u, 
	638u, 640u, 767u, 269u, 288u, 300u, 303u, 347u, 
	348u, 349u, 379u, 381u, 525u, 544u, 556u, 559u, 
	603u, 604u, 605u, 635u, 637u, 639u, 289u, 294u, 
	295u, 297u, 298u, 313u, 314u, 320u, 321u, 382u, 
	384u, 511u, 512u, 543u, 545u, 550u, 551u, 553u, 
	554u, 569u, 570u, 576u, 577u, 638u, 640u, 767u, 
	269u, 525u, 639u, 288u, 382u, 384u, 511u, 512u, 
	543u, 544u, 767u, 48u, 49u, 57u, 65u, 70u, 
	97u, 102u, 269u, 304u, 525u, 560u, 305u, 313u, 
	321u, 326u, 353u, 358u, 512u, 559u, 561u, 569u, 
	570u, 576u, 577u, 582u, 583u, 608u, 609u, 614u, 
	615u, 767u, 269u, 304u, 380u, 382u, 525u, 556u, 
	559u, 560u, 635u, 637u, 289u, 294u, 298u, 299u, 
	301u, 302u, 305u, 313u, 321u, 326u, 327u, 346u, 
	350u, 352u, 353u, 358u, 359u, 378u, 512u, 544u, 
	545u, 550u, 551u, 553u, 554u, 558u, 561u, 569u, 
	570u, 576u, 577u, 582u, 583u, 602u, 603u, 605u, 
	606u, 608u, 609u, 614u, 615u, 638u, 639u, 767u, 
	13u, 48u, 124u, 126u, 33u, 38u, 42u, 43u, 
	45u, 46u, 49u, 57u, 65u, 70u, 71u, 90u, 
	94u, 96u, 97u, 102u, 103u, 122u, 269u, 304u, 
	380u, 382u, 525u, 556u, 559u, 560u, 635u, 637u, 
	289u, 294u, 298u, 299u, 301u, 302u, 305u, 313u, 
	321u, 326u, 327u, 346u, 350u, 352u, 353u, 358u, 
	359u, 378u, 512u, 544u, 545u, 550u, 551u, 553u, 
	554u, 558u, 561u, 569u, 570u, 576u, 577u, 582u, 
	583u, 602u, 603u, 605u, 606u, 608u, 609u, 614u, 
	615u, 638u, 639u, 767u, 269u, 304u, 525u, 560u, 
	305u, 313u, 321u, 326u, 353u, 358u, 512u, 559u, 
	561u, 569u, 570u, 576u, 577u, 582u, 583u, 608u, 
	609u, 614u, 615u, 767u, 0
};

static const char _chunk_parser_single_lengths[] = {
	0, 1, 2, 1, 8, 1, 2, 1, 
	2, 2, 4, 4, 1, 3, 3, 1, 
	3, 2, 5, 5, 2, 5, 3, 4, 
	7, 3, 2, 2, 5, 5, 8, 4, 
	2, 5, 3, 4, 7, 3, 2, 2, 
	5, 5, 8, 4, 10, 5, 1, 4, 
	12, 4, 12, 1, 4, 4, 8, 12, 
	10, 10, 23, 7, 4, 6, 4, 9, 
	9, 21, 25, 19, 8, 12, 10, 10, 
	23, 7, 4, 6, 4, 9, 9, 21, 
	25, 19, 3, 1, 4, 10, 4, 10, 
	4
};

static const char _chunk_parser_range_lengths[] = {
	0, 3, 3, 0, 15, 0, 3, 0, 
	1, 1, 1, 10, 0, 6, 1, 0, 
	6, 1, 9, 9, 6, 6, 6, 6, 
	7, 1, 0, 0, 1, 7, 7, 7, 
	6, 6, 6, 6, 7, 1, 0, 0, 
	1, 7, 7, 7, 15, 4, 0, 1, 
	22, 1, 22, 0, 10, 1, 15, 15, 
	15, 15, 13, 4, 1, 2, 1, 4, 
	4, 13, 13, 13, 15, 15, 15, 15, 
	13, 4, 1, 2, 1, 4, 4, 13, 
	13, 13, 4, 3, 10, 22, 9, 22, 
	10
};

static const short _chunk_parser_index_offsets[] = {
	0, 0, 5, 11, 13, 37, 39, 45, 
	47, 51, 55, 61, 76, 78, 88, 93, 
	95, 105, 109, 124, 139, 148, 160, 170, 
	181, 196, 201, 204, 207, 214, 227, 243, 
	255, 264, 276, 286, 297, 312, 317, 320, 
	323, 330, 343, 359, 371, 397, 407, 409, 
	415, 450, 456, 491, 493, 508, 514, 538, 
	566, 592, 618, 655, 667, 673, 682, 688, 
	702, 716, 751, 790, 823, 847, 875, 901, 
	927, 964, 976, 982, 991, 997, 1011, 1025, 
	1060, 1099, 1132, 1140, 1145, 1160, 1193, 1207, 
	1240
};

static const char _chunk_parser_indicies[] = {
	0, 2, 2, 2, 1, 3, 5, 4, 
	4, 4, 1, 6, 1, 7, 8, 8, 
	10, 9, 9, 9, 9, 8, 8, 8, 
	8, 8, 8, 9, 11, 9, 11, 9, 
	11, 9, 11, 9, 1, 12, 1, 13, 
	14, 4, 4, 4, 1, 15, 1, 7, 
	10, 9, 1, 16, 18, 17, 1, 12, 
	16, 19, 18, 17, 1, 20, 5, 21, 
	23, 4, 4, 4, 17, 22, 17, 22, 
	17, 22, 17, 1, 24, 1, 26, 25, 
	25, 25, 25, 25, 25, 25, 25, 1, 
	27, 28, 1, 1, 29, 30, 1, 16, 
	8, 8, 8, 8, 8, 8, 8, 8, 
	1, 31, 1, 1, 32, 3, 26, 5, 
	25, 25, 25, 25, 25, 33, 33, 25, 
	25, 33, 25, 1, 13, 26, 14, 25, 
	25, 25, 25, 25, 33, 33, 25, 25, 
	33, 25, 1, 34, 34, 34, 34, 34, 
	34, 34, 34, 1, 35, 36, 37, 34, 
	34, 34, 34, 34, 34, 34, 34, 1, 
	39, 38, 38, 38, 38, 38, 38, 38, 
	38, 1, 35, 36, 38, 38, 38, 38, 
	38, 38, 38, 38, 1, 35, 38, 41, 
	42, 39, 39, 1, 1, 39, 39, 39, 
	39, 39, 39, 40, 43, 42, 1, 1, 
	40, 35, 36, 1, 44, 42, 40, 35, 
	43, 41, 42, 1, 1, 40, 34, 42, 
	45, 45, 1, 1, 45, 45, 45, 45, 
	45, 45, 40, 35, 34, 41, 46, 42, 
	45, 45, 1, 1, 45, 45, 45, 45, 
	45, 45, 40, 42, 39, 39, 1, 1, 
	39, 39, 39, 39, 39, 39, 40, 47, 
	47, 47, 47, 47, 47, 47, 47, 1, 
	48, 49, 50, 47, 47, 47, 47, 47, 
	47, 47, 47, 1, 52, 51, 51, 51, 
	51, 51, 51, 51, 51, 1, 48, 49, 
	51, 51, 51, 51, 51, 51, 51, 51, 
	1, 48, 51, 54, 55, 52, 52, 1, 
	1, 52, 52, 52, 52, 52, 52, 53, 
	56, 55, 1, 1, 53, 48, 49, 1, 
	57, 55, 53, 48, 56, 54, 55, 1, 
	1, 53, 47, 55, 58, 58, 1, 1, 
	58, 58, 58, 58, 58, 58, 53, 48, 
	47, 54, 59, 55, 58, 58, 1, 1, 
	58, 58, 58, 58, 58, 58, 53, 55, 
	52, 52, 1, 1, 52, 52, 52, 52, 
	52, 52, 53, 16, 26, 25, 25, 18, 
	17, 17, 61, 17, 17, 25, 25, 25, 
	25, 25, 25, 17, 60, 17, 60, 17, 
	60, 17, 60, 17, 1, 62, 28, 63, 
	64, 17, 29, 29, 17, 65, 1, 66, 
	1, 66, 16, 67, 18, 17, 1, 20, 
	26, 5, 25, 25, 21, 17, 17, 61, 
	23, 17, 17, 25, 25, 25, 33, 33, 
	25, 25, 33, 25, 17, 60, 17, 60, 
	68, 17, 68, 60, 17, 60, 68, 60, 
	17, 1, 24, 16, 24, 18, 17, 1, 
	69, 26, 14, 25, 25, 70, 17, 17, 
	61, 71, 17, 17, 25, 25, 25, 33, 
	33, 25, 25, 33, 25, 17, 60, 17, 
	60, 68, 17, 68, 60, 17, 60, 68, 
	60, 17, 1, 72, 1, 69, 14, 70, 
	71, 4, 4, 4, 17, 22, 17, 22, 
	17, 22, 17, 1, 72, 16, 72, 18, 
	17, 1, 16, 34, 34, 18, 17, 17, 
	17, 17, 34, 34, 34, 34, 34, 34, 
	17, 73, 17, 73, 17, 73, 17, 73, 
	17, 1, 74, 36, 37, 34, 34, 75, 
	17, 17, 76, 77, 17, 17, 34, 34, 
	34, 34, 34, 34, 17, 73, 17, 73, 
	17, 73, 17, 73, 17, 1, 16, 39, 
	38, 38, 18, 79, 17, 17, 17, 17, 
	38, 38, 38, 38, 38, 38, 17, 78, 
	17, 78, 17, 78, 17, 78, 17, 1, 
	74, 36, 38, 38, 75, 17, 17, 76, 
	17, 17, 38, 38, 38, 38, 38, 38, 
	17, 78, 17, 78, 17, 78, 17, 78, 
	17, 1, 74, 40, 38, 40, 40, 41, 
	40, 42, 40, 40, 40, 75, 80, 78, 
	80, 80, 81, 80, 82, 80, 80, 80, 
	17, 39, 40, 39, 40, 39, 40, 17, 
	79, 80, 79, 80, 79, 80, 1, 16, 
	43, 42, 18, 83, 82, 17, 40, 40, 
	17, 80, 1, 74, 36, 75, 76, 17, 
	1, 84, 44, 42, 85, 86, 82, 40, 
	80, 1, 12, 43, 42, 1, 1, 40, 
	12, 16, 43, 42, 19, 18, 83, 82, 
	17, 40, 40, 17, 80, 1, 74, 43, 
	41, 42, 75, 83, 81, 82, 17, 40, 
	40, 17, 80, 1, 16, 40, 34, 40, 
	40, 40, 42, 40, 40, 40, 18, 80, 
	73, 80, 80, 80, 82, 80, 80, 80, 
	17, 45, 40, 45, 40, 45, 40, 17, 
	87, 80, 87, 80, 87, 80, 1, 74, 
	40, 34, 40, 40, 41, 46, 40, 42, 
	40, 40, 40, 75, 80, 73, 80, 80, 
	81, 88, 80, 82, 80, 80, 80, 17, 
	45, 40, 45, 40, 45, 40, 17, 87, 
	80, 87, 80, 87, 80, 1, 16, 40, 
	40, 40, 40, 42, 40, 40, 40, 18, 
	80, 80, 80, 80, 82, 80, 80, 80, 
	17, 39, 40, 39, 40, 39, 40, 17, 
	79, 80, 79, 80, 79, 80, 1, 16, 
	47, 47, 18, 17, 17, 17, 17, 47, 
	47, 47, 47, 47, 47, 17, 89, 17, 
	89, 17, 89, 17, 89, 17, 1, 90, 
	49, 50, 47, 47, 91, 17, 17, 92, 
	93, 17, 17, 47, 47, 47, 47, 47, 
	47, 17, 89, 17, 89, 17, 89, 17, 
	89, 17, 1, 16, 52, 51, 51, 18, 
	95, 17, 17, 17, 17, 51, 51, 51, 
	51, 51, 51, 17, 94, 17, 94, 17, 
	94, 17, 94, 17, 1, 90, 49, 51, 
	51, 91, 17, 17, 92, 17, 17, 51, 
	51, 51, 51, 51, 51, 17, 94, 17, 
	94, 17, 94, 17, 94, 17, 1, 90, 
	53, 51, 53, 53, 54, 53, 55, 53, 
	53, 53, 91, 96, 94, 96, 96, 97, 
	96, 98, 96, 96, 96, 17, 52, 53, 
	52, 53, 52, 53, 17, 95, 96, 95, 
	96, 95, 96, 1, 16, 56, 55, 18, 
	99, 98, 17, 53, 53, 17, 96, 1, 
	90, 49, 91, 92, 17, 1, 100, 57, 
	55, 101, 102, 98, 53, 96, 1, 12, 
	56, 55, 1, 1, 53, 12, 16, 56, 
	55, 19, 18, 99, 98, 17, 53, 53, 
	17, 96, 1, 90, 56, 54, 55, 91, 
	99, 97, 98, 17, 53, 53, 17, 96, 
	1, 16, 53, 47, 53, 53, 53, 55, 
	53, 53, 53, 18, 96, 89, 96, 96, 
	96, 98, 96, 96, 96, 17, 58, 53, 
	58, 53, 58, 53, 17, 103, 96, 103, 
	96, 103, 96, 1, 90, 53, 47, 53, 
	53, 54, 59, 53, 55, 53, 53, 53, 
	91, 96, 89, 96, 96, 97, 104, 96, 
	98, 96, 96, 96, 17, 58, 53, 58, 
	53, 58, 53, 17, 103, 96, 103, 96, 
	103, 96, 1, 16, 53, 53, 53, 53, 
	55, 53, 53, 53, 18, 96, 96, 96, 
	96, 98, 96, 96, 96, 17, 52, 53, 
	52, 53, 52, 53, 17, 95, 96, 95, 
	96, 95, 96, 1, 105, 106, 17, 32, 
	32, 17, 107, 1, 0, 2, 2, 2, 
	1, 16, 0, 18, 108, 2, 2, 2, 
	17, 109, 17, 109, 17, 109, 17, 1, 
	7, 110, 8, 8, 10, 9, 9, 112, 
	9, 9, 8, 8, 8, 111, 111, 8, 
	8, 111, 8, 9, 11, 9, 11, 113, 
	9, 113, 11, 9, 11, 113, 11, 9, 
	1, 16, 110, 8, 8, 8, 8, 8, 
	111, 111, 8, 8, 111, 8, 1, 16, 
	110, 8, 8, 18, 17, 17, 115, 17, 
	17, 8, 8, 8, 111, 111, 8, 8, 
	111, 8, 17, 114, 17, 114, 116, 17, 
	116, 114, 17, 114, 116, 114, 17, 1, 
	7, 0, 10, 117, 2, 2, 2, 9, 
	118, 9, 118, 9, 118, 9, 1, 0
};

static const char _chunk_parser_trans_targs[] = {
	2, 0, 6, 3, 6, 32, 4, 5, 
	13, 9, 10, 44, 83, 7, 20, 8, 
	5, 9, 10, 84, 12, 49, 52, 68, 
	85, 13, 14, 15, 14, 17, 16, 15, 
	17, 19, 21, 7, 20, 22, 23, 24, 
	25, 29, 27, 26, 28, 30, 31, 33, 
	3, 32, 34, 35, 36, 37, 41, 39, 
	38, 40, 42, 43, 44, 45, 46, 47, 
	45, 82, 86, 87, 50, 51, 53, 54, 
	88, 55, 51, 53, 54, 56, 57, 58, 
	59, 65, 61, 60, 62, 63, 64, 66, 
	67, 69, 12, 49, 68, 70, 71, 72, 
	73, 79, 75, 74, 76, 77, 78, 80, 
	81, 46, 47, 82, 11, 52, 18, 19, 
	48, 50, 44, 48, 50, 11, 52
};

static const char _chunk_parser_trans_actions[] = {
	9, 0, 9, 29, 0, 29, 0, 13, 
	1, 13, 13, 20, 15, 11, 11, 0, 
	0, 0, 0, 15, 29, 29, 0, 29, 
	15, 0, 3, 23, 5, 5, 0, 7, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 3, 23, 23, 
	5, 5, 15, 15, 0, 11, 11, 11, 
	15, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 7, 7, 0, 9, 9, 17, 17, 
	32, 32, 1, 17, 17, 26, 26
};

static const int chunk_parser_start = 1;
static const int chunk_parser_first_final = 83;
static const int chunk_parser_error = 0;

static const int chunk_parser_en_main = 1;


/* #line 639 "http_message_parser.cpp" */
	{
	cs = chunk_parser_start;
	}

/* #line 642 "http_message_parser.cpp" */
	{
	int _klen;
	unsigned int _trans;
	short _widec;
	const char *_acts;
	unsigned int _nacts;
	const short *_keys;

	if ( cs == 0 )
		goto _out;
_resume:
	_widec = (*p);
	_klen = _chunk_parser_cond_lengths[cs];
	_keys = _chunk_parser_cond_keys + (_chunk_parser_cond_offsets[cs]*2);
	if ( _klen > 0 ) {
		const short *_lower = _keys;
		const short *_mid;
		const short *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( _widec < _mid[0] )
				_upper = _mid - 2;
			else if ( _widec > _mid[1] )
				_lower = _mid + 2;
			else {
				switch ( _chunk_parser_cond_spaces[_chunk_parser_cond_offsets[cs] + ((_mid - _keys)>>1)] ) {
	case 0: {
		_widec = (short)(256u + ((*p) - 0u));
		if ( 
/* #line 98 "rfc2616.rl" */
 seen_chunk_size++ < chunk_size  ) _widec += 256;
		break;
	}
				}
				break;
			}
		}
	}

	_keys = _chunk_parser_trans_keys + _chunk_parser_key_offsets[cs];
	_trans = _chunk_parser_index_offsets[cs];

	_klen = _chunk_parser_single_lengths[cs];
	if ( _klen > 0 ) {
		const short *_lower = _keys;
		const short *_mid;
		const short *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( _widec < *_mid )
				_upper = _mid - 1;
			else if ( _widec > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _chunk_parser_range_lengths[cs];
	if ( _klen > 0 ) {
		const short *_lower = _keys;
		const short *_mid;
		const short *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( _widec < _mid[0] )
				_upper = _mid - 2;
			else if ( _widec > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _chunk_parser_indicies[_trans];
	cs = _chunk_parser_trans_targs[_trans];

	if ( _chunk_parser_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _chunk_parser_actions + _chunk_parser_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
/* #line 81 "rfc2616.rl" */
	{ field_name.iov_base = p; }
	break;
	case 1:
/* #line 81 "rfc2616.rl" */
	{ field_name.iov_len = p - static_cast<char*>(field_name.iov_base); }
	break;
	case 2:
/* #line 81 "rfc2616.rl" */
	{ field_value.iov_base = p; }
	break;
	case 3:
/* #line 81 "rfc2616.rl" */
	{ field_value.iov_len = p - static_cast<char*>(field_value.iov_base); }
	break;
	case 4:
/* #line 82 "rfc2616.rl" */
	{ chunk_size_p = p; }
	break;
	case 5:
/* #line 84 "rfc2616.rl" */
	{
                 char* chunk_size_pe = p;
                 chunk_size
                   = static_cast<size_t>(
                       strtol(chunk_size_p, &chunk_size_pe, 16)
                     );
               }
	break;
	case 6:
/* #line 99 "rfc2616.rl" */
	{ chunk_data_p = p; }
	break;
	case 7:
/* #line 105 "rfc2616.rl" */
	{ chunk_size = 0; }
	break;
	case 8:
/* #line 97 "http_message_parser.rl" */
	{
    if (chunk_size > 0) {
      // Cut off the chunk size + extension + CRLF before
      // the chunk data and the CRLF after
      callbacks.handle_http_message_body_chunk(
	    Buffer::copy(chunk_data_p, p - chunk_data_p - 2)
      );
    } else { // Last chunk
      callbacks.handle_http_message_body_chunk(NULL);
      return;
	}
    }
	break;
/* #line 788 "http_message_parser.cpp" */
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	p += 1;
	goto _resume;
	_out: {}
	}

/* #line 114 "http_message_parser.rl" */


  CHECK(cs == chunk_parser_error);
  if (p == eof && chunk_size != 0) {
    callbacks.read(::std::make_shared<Buffer>(chunk_size + 2)); // Assumes no trailers.
  }
}

bool
HttpMessageParser::parse_content_length_field(
  const char* ps,
  const char* pe,
  size_t& content_length
) {
  int cs;
  char* p = const_cast<char*>(ps);

  iovec field_name = {0, 0}, field_value = {0, 0};

  // Don't look for the trailing CRLF before the body,
  // since it may not be present yet.
  
/* #line 819 "http_message_parser.cpp" */
static const char _content_length_field_parser_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 2, 2, 3
};

static const char _content_length_field_parser_key_offsets[] = {
	0, 0, 15, 20, 21, 25
};

static const unsigned char _content_length_field_parser_trans_keys[] = {
	58u, 124u, 126u, 33u, 38u, 42u, 43u, 45u, 
	46u, 48u, 57u, 65u, 90u, 94u, 122u, 13u, 
	32u, 127u, 0u, 31u, 10u, 13u, 127u, 0u, 
	31u, 124u, 126u, 33u, 38u, 42u, 43u, 45u, 
	46u, 48u, 57u, 65u, 90u, 94u, 122u, 0
};

static const char _content_length_field_parser_single_lengths[] = {
	0, 3, 3, 1, 2, 2
};

static const char _content_length_field_parser_range_lengths[] = {
	0, 6, 1, 0, 1, 6
};

static const char _content_length_field_parser_index_offsets[] = {
	0, 0, 10, 15, 17, 21
};

static const char _content_length_field_parser_indicies[] = {
	2, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 3, 4, 1, 1, 5, 6, 
	1, 7, 1, 1, 8, 9, 9, 9, 
	9, 9, 9, 9, 9, 1, 0
};

static const char _content_length_field_parser_trans_targs[] = {
	1, 0, 2, 3, 2, 4, 5, 3, 
	4, 1
};

static const char _content_length_field_parser_trans_actions[] = {
	0, 0, 3, 11, 5, 5, 9, 7, 
	0, 1
};

static const int content_length_field_parser_start = 5;
static const int content_length_field_parser_first_final = 5;
static const int content_length_field_parser_error = 0;

static const int content_length_field_parser_en_main = 5;


/* #line 871 "http_message_parser.cpp" */
	{
	cs = content_length_field_parser_start;
	}

/* #line 874 "http_message_parser.cpp" */
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const unsigned char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _content_length_field_parser_trans_keys + _content_length_field_parser_key_offsets[cs];
	_trans = _content_length_field_parser_index_offsets[cs];

	_klen = _content_length_field_parser_single_lengths[cs];
	if ( _klen > 0 ) {
		const unsigned char *_lower = _keys;
		const unsigned char *_mid;
		const unsigned char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _content_length_field_parser_range_lengths[cs];
	if ( _klen > 0 ) {
		const unsigned char *_lower = _keys;
		const unsigned char *_mid;
		const unsigned char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _content_length_field_parser_indicies[_trans];
	cs = _content_length_field_parser_trans_targs[_trans];

	if ( _content_length_field_parser_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _content_length_field_parser_actions + _content_length_field_parser_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
/* #line 81 "rfc2616.rl" */
	{ field_name.iov_base = p; }
	break;
	case 1:
/* #line 81 "rfc2616.rl" */
	{ field_name.iov_len = p - static_cast<char*>(field_name.iov_base); }
	break;
	case 2:
/* #line 81 "rfc2616.rl" */
	{ field_value.iov_base = p; }
	break;
	case 3:
/* #line 81 "rfc2616.rl" */
	{ field_value.iov_len = p - static_cast<char*>(field_value.iov_base); }
	break;
	case 4:
/* #line 142 "http_message_parser.rl" */
	{
        if (
          parse_content_length_field(
            field_name,
            field_value,
            content_length
          )
        )
          return true;
      }
	break;
/* #line 971 "http_message_parser.cpp" */
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

/* #line 157 "http_message_parser.rl" */


  return false;
}

bool
HttpMessageParser::parse_content_length_field(
  const iovec& field_name,
  const iovec& field_value,
  size_t& content_length
) {
  if (
    field_name.iov_len == 14
    &&
    (
      memcmp(field_name.iov_base, "Content-Length", 14) == 0
      ||
      memcmp(field_name.iov_base, "Content-length", 14) == 0
    )
  ) {
    char* nptr = static_cast<char*>(field_value.iov_base);
    char* endptr = nptr + field_value.iov_len;
    content_length = static_cast<size_t>(strtol(nptr, &endptr, 10));
    return true;
  }
  else if (
    field_name.iov_len == 17
    && (
      memcmp(field_name.iov_base, "Transfer-Encoding", 17) == 0
      ||
      memcmp(field_name.iov_base, "Transfer-encoding", 17) == 0
    )
    &&
    memcmp(field_value.iov_base, "chunked", 7) == 0
  ) {
    content_length = HttpRequest::CONTENT_LENGTH_CHUNKED;
    return true;
  } else
    return false;
}

DateTime HttpMessageParser::parse_date(const iovec& date) {
return parse_date(
          static_cast<const char*>(date.iov_base),
          static_cast<const char*>(date.iov_base) + date.iov_len
        );
}

DateTime HttpMessageParser::parse_date(const char* ps, const char* pe) {
  int cs;
  const char* eof = pe;
  char* p = const_cast<char*>(ps);

  int hour = 0, minute = 0, second = 0;
  int day = 0, month = 0, year = 0;

  
/* #line 1038 "http_message_parser.cpp" */
static const char _date_parser_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	7, 1, 8
};

static const unsigned char _date_parser_key_offsets[] = {
	0, 0, 5, 6, 7, 10, 18, 20, 
	21, 22, 25, 27, 28, 30, 32, 33, 
	35, 37, 38, 40, 42, 43, 45, 47, 
	49, 51, 53, 54, 55, 56, 57, 58, 
	59, 61, 62, 64, 65, 67, 68, 69, 
	70, 71, 72, 73, 74, 76, 78, 79, 
	87, 89, 90, 91, 93, 95, 97, 99, 
	100, 102, 104, 105, 107, 109, 110, 112, 
	114, 115, 116, 117, 118, 119, 120, 121, 
	122, 123, 124, 126, 127, 129, 130, 132, 
	133, 134, 135, 136, 137, 138, 139, 140, 
	141, 142, 144, 146, 147, 155, 157, 158, 
	159, 161, 162, 163, 164, 165, 166, 167, 
	169, 170, 172, 173, 175, 176, 177, 178, 
	179, 180, 181, 182, 183, 185, 186, 189, 
	190, 191, 193, 194, 197, 198, 199, 202, 
	203, 204, 207, 208
};

static const unsigned char _date_parser_trans_keys[] = {
	70u, 77u, 83u, 84u, 87u, 114u, 105u, 32u, 
	44u, 100u, 65u, 68u, 70u, 74u, 77u, 78u, 
	79u, 83u, 112u, 117u, 114u, 32u, 32u, 48u, 
	57u, 48u, 57u, 32u, 48u, 57u, 48u, 57u, 
	58u, 48u, 57u, 48u, 57u, 58u, 48u, 57u, 
	48u, 57u, 32u, 48u, 57u, 48u, 57u, 48u, 
	57u, 48u, 57u, 48u, 57u, 103u, 101u, 99u, 
	32u, 101u, 98u, 97u, 117u, 110u, 108u, 110u, 
	97u, 114u, 121u, 111u, 118u, 99u, 116u, 101u, 
	112u, 32u, 48u, 57u, 48u, 57u, 32u, 65u, 
	68u, 70u, 74u, 77u, 78u, 79u, 83u, 112u, 
	117u, 114u, 32u, 48u, 57u, 48u, 57u, 48u, 
	57u, 48u, 57u, 32u, 48u, 57u, 48u, 57u, 
	58u, 48u, 57u, 48u, 57u, 58u, 48u, 57u, 
	48u, 57u, 32u, 71u, 77u, 84u, 103u, 101u, 
	99u, 32u, 101u, 98u, 97u, 117u, 110u, 108u, 
	110u, 97u, 114u, 121u, 111u, 118u, 99u, 116u, 
	101u, 112u, 97u, 121u, 44u, 32u, 48u, 57u, 
	48u, 57u, 45u, 65u, 68u, 70u, 74u, 77u, 
	78u, 79u, 83u, 112u, 117u, 114u, 45u, 48u, 
	57u, 103u, 101u, 99u, 45u, 101u, 98u, 97u, 
	117u, 110u, 108u, 110u, 97u, 114u, 121u, 111u, 
	118u, 99u, 116u, 101u, 112u, 111u, 110u, 97u, 
	117u, 116u, 32u, 44u, 117u, 114u, 100u, 104u, 
	117u, 117u, 32u, 44u, 114u, 115u, 101u, 32u, 
	44u, 115u, 101u, 100u, 32u, 44u, 110u, 101u, 
	0
};

static const char _date_parser_single_lengths[] = {
	0, 5, 1, 1, 3, 8, 2, 1, 
	1, 1, 0, 1, 0, 0, 1, 0, 
	0, 1, 0, 0, 1, 0, 0, 0, 
	0, 0, 1, 1, 1, 1, 1, 1, 
	2, 1, 2, 1, 2, 1, 1, 1, 
	1, 1, 1, 1, 0, 0, 1, 8, 
	2, 1, 1, 0, 0, 0, 0, 1, 
	0, 0, 1, 0, 0, 1, 0, 0, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 2, 1, 2, 1, 2, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 0, 0, 1, 8, 2, 1, 1, 
	0, 1, 1, 1, 1, 1, 1, 2, 
	1, 2, 1, 2, 1, 1, 1, 1, 
	1, 1, 1, 1, 2, 1, 3, 1, 
	1, 2, 1, 3, 1, 1, 3, 1, 
	1, 3, 1, 0
};

static const char _date_parser_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 1, 0, 1, 1, 0, 1, 
	1, 0, 1, 1, 0, 1, 1, 1, 
	1, 1, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 1, 1, 0, 0, 
	0, 0, 0, 1, 1, 1, 1, 0, 
	1, 1, 0, 1, 1, 0, 1, 1, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 1, 0, 0, 0, 0, 0, 
	1, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0
};

static const short _date_parser_index_offsets[] = {
	0, 0, 6, 8, 10, 14, 23, 26, 
	28, 30, 33, 35, 37, 39, 41, 43, 
	45, 47, 49, 51, 53, 55, 57, 59, 
	61, 63, 65, 67, 69, 71, 73, 75, 
	77, 80, 82, 85, 87, 90, 92, 94, 
	96, 98, 100, 102, 104, 106, 108, 110, 
	119, 122, 124, 126, 128, 130, 132, 134, 
	136, 138, 140, 142, 144, 146, 148, 150, 
	152, 154, 156, 158, 160, 162, 164, 166, 
	168, 170, 172, 175, 177, 180, 182, 185, 
	187, 189, 191, 193, 195, 197, 199, 201, 
	203, 205, 207, 209, 211, 220, 223, 225, 
	227, 229, 231, 233, 235, 237, 239, 241, 
	244, 246, 249, 251, 254, 256, 258, 260, 
	262, 264, 266, 268, 270, 273, 275, 279, 
	281, 283, 286, 288, 292, 294, 296, 300, 
	302, 304, 308, 310
};

static const unsigned char _date_parser_indicies[] = {
	1, 2, 3, 4, 5, 0, 6, 0, 
	7, 0, 8, 9, 10, 0, 11, 12, 
	13, 14, 15, 16, 17, 18, 0, 19, 
	20, 0, 21, 0, 22, 0, 23, 24, 
	0, 25, 0, 26, 0, 27, 0, 28, 
	0, 29, 0, 30, 0, 31, 0, 32, 
	0, 33, 0, 34, 0, 35, 0, 36, 
	0, 37, 0, 38, 0, 39, 0, 40, 
	0, 21, 0, 41, 0, 42, 0, 43, 
	0, 44, 0, 21, 0, 45, 46, 0, 
	21, 0, 21, 21, 0, 47, 0, 21, 
	21, 0, 48, 0, 21, 0, 49, 0, 
	21, 0, 50, 0, 21, 0, 51, 0, 
	52, 0, 53, 0, 54, 0, 55, 56, 
	57, 58, 59, 60, 61, 62, 0, 63, 
	64, 0, 65, 0, 66, 0, 67, 0, 
	68, 0, 69, 0, 70, 0, 71, 0, 
	72, 0, 73, 0, 74, 0, 75, 0, 
	76, 0, 77, 0, 78, 0, 79, 0, 
	80, 0, 81, 0, 82, 0, 39, 0, 
	65, 0, 83, 0, 84, 0, 85, 0, 
	86, 0, 65, 0, 87, 88, 0, 65, 
	0, 65, 65, 0, 89, 0, 65, 65, 
	0, 90, 0, 65, 0, 91, 0, 65, 
	0, 92, 0, 65, 0, 93, 0, 94, 
	0, 95, 0, 96, 0, 97, 0, 98, 
	0, 99, 0, 100, 101, 102, 103, 104, 
	105, 106, 107, 0, 108, 109, 0, 110, 
	0, 111, 0, 112, 0, 110, 0, 113, 
	0, 114, 0, 115, 0, 116, 0, 110, 
	0, 117, 118, 0, 110, 0, 110, 110, 
	0, 119, 0, 110, 110, 0, 120, 0, 
	110, 0, 121, 0, 110, 0, 122, 0, 
	110, 0, 123, 0, 7, 0, 124, 123, 
	0, 125, 0, 8, 9, 126, 0, 127, 
	0, 10, 0, 128, 129, 0, 130, 0, 
	8, 9, 131, 0, 127, 0, 132, 0, 
	8, 9, 127, 0, 133, 0, 134, 0, 
	8, 9, 135, 0, 131, 0, 0, 0
};

static const unsigned char _date_parser_trans_targs[] = {
	0, 2, 114, 116, 121, 127, 3, 4, 
	5, 43, 85, 6, 27, 30, 32, 35, 
	37, 39, 41, 7, 26, 8, 9, 10, 
	25, 11, 12, 13, 14, 15, 16, 17, 
	18, 19, 20, 21, 22, 23, 24, 131, 
	11, 28, 29, 9, 31, 33, 34, 36, 
	38, 40, 42, 44, 45, 46, 47, 48, 
	69, 72, 74, 77, 79, 81, 83, 49, 
	68, 50, 51, 52, 53, 54, 55, 56, 
	57, 58, 59, 60, 61, 62, 63, 64, 
	65, 66, 67, 70, 71, 51, 73, 75, 
	76, 78, 80, 82, 84, 86, 87, 88, 
	89, 90, 91, 92, 93, 98, 101, 103, 
	106, 108, 110, 112, 94, 97, 95, 96, 
	54, 99, 100, 96, 102, 104, 105, 107, 
	109, 111, 113, 115, 117, 118, 119, 120, 
	122, 125, 123, 124, 126, 128, 129, 130
};

static const char _date_parser_trans_actions[] = {
	17, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	9, 7, 0, 1, 0, 0, 3, 0, 
	0, 5, 0, 0, 15, 0, 0, 0, 
	0, 0, 0, 11, 0, 0, 0, 0, 
	0, 0, 0, 0, 9, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 15, 0, 0, 0, 0, 
	1, 0, 0, 3, 0, 0, 5, 0, 
	0, 0, 0, 0, 0, 11, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 9, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	13, 0, 0, 11, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0
};

static const char _date_parser_eof_actions[] = {
	0, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 0
};

static const int date_parser_start = 1;
static const int date_parser_first_final = 131;
static const int date_parser_error = 0;

static const int date_parser_en_main = 1;


/* #line 1262 "http_message_parser.cpp" */
	{
	cs = date_parser_start;
	}

/* #line 1265 "http_message_parser.cpp" */
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const unsigned char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _date_parser_trans_keys + _date_parser_key_offsets[cs];
	_trans = _date_parser_index_offsets[cs];

	_klen = _date_parser_single_lengths[cs];
	if ( _klen > 0 ) {
		const unsigned char *_lower = _keys;
		const unsigned char *_mid;
		const unsigned char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _date_parser_range_lengths[cs];
	if ( _klen > 0 ) {
		const unsigned char *_lower = _keys;
		const unsigned char *_mid;
		const unsigned char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _date_parser_indicies[_trans];
	cs = _date_parser_trans_targs[_trans];

	if ( _date_parser_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _date_parser_actions + _date_parser_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
/* #line 33 "rfc2616.rl" */
	{ hour = atoi(p); }
	break;
	case 1:
/* #line 34 "rfc2616.rl" */
	{ minute = atoi(p); }
	break;
	case 2:
/* #line 35 "rfc2616.rl" */
	{ second = atoi(p); }
	break;
	case 3:
/* #line 37 "rfc2616.rl" */
	{ day = atoi(p); }
	break;
	case 4:
/* #line 38 "rfc2616.rl" */
	{ day = atoi(p); }
	break;
	case 5:
/* #line 45 "rfc2616.rl" */
	{
            switch (*(p - 1))
            {
              case 'b': month = 2; break;
              case 'c': month = 12; break;
              case 'l': month = 7; break;
              case 'g': month = 8; break;
              case 'n':
              {
                switch ((*p - 2))
                {
                  case 'a': month = 1; break;
                  case 'u': month = 6; break;
                }
              }
              break;
              case 'p': month = 9; break;
              case 'r':
              {
                switch (*(p - 2))
                {
                  case 'a': month = 3; break;
                  case 'p': month = 4; break;
                }
              }
              break;
              case 't': month = 10; break;
              case 'v': month = 11; break;
              case 'y': month = 5; break;
            }
          }
	break;
	case 6:
/* #line 76 "rfc2616.rl" */
	{ year = atoi(p); year += (year < 50 ? 2000 : 1900); }
	break;
	case 7:
/* #line 77 "rfc2616.rl" */
	{ year = atoi(p); }
	break;
	case 8:
/* #line 220 "http_message_parser.rl" */
	{ return DateTime::INVALID_DATE_TIME; }
	break;
/* #line 1395 "http_message_parser.cpp" */
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	const char *__acts = _date_parser_actions + _date_parser_eof_actions[cs];
	unsigned int __nacts = (unsigned int) *__acts++;
	while ( __nacts-- > 0 ) {
		switch ( *__acts++ ) {
	case 8:
/* #line 220 "http_message_parser.rl" */
	{ return DateTime::INVALID_DATE_TIME; }
	break;
/* #line 1413 "http_message_parser.cpp" */
		}
	}
	}

	_out: {}
	}

/* #line 225 "http_message_parser.rl" */


  if (cs != date_parser_error) {
    if (year < 100) year += 2000;
    year -= 1900;
    return DateTime(second, minute, hour, day, month - 1, year, false);
  } else
    return DateTime::INVALID_DATE_TIME;
}

bool
HttpMessageParser::parse_field(
  const char* ps,
  const char* pe,
  const iovec& in_field_name,
  iovec& out_field_value
) {
  int cs;
  char* p = const_cast<char*>(ps);

  iovec field_name = {0, 0}, field_value = {0, 0};

  // Don't look for the trailing CRLF before the body,
  // since it may not be present yet.
  
/* #line 1443 "http_message_parser.cpp" */
static const char _field_parser_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 2, 2, 3
};

static const char _field_parser_key_offsets[] = {
	0, 0, 15, 20, 21, 25
};

static const unsigned char _field_parser_trans_keys[] = {
	58u, 124u, 126u, 33u, 38u, 42u, 43u, 45u, 
	46u, 48u, 57u, 65u, 90u, 94u, 122u, 13u, 
	32u, 127u, 0u, 31u, 10u, 13u, 127u, 0u, 
	31u, 124u, 126u, 33u, 38u, 42u, 43u, 45u, 
	46u, 48u, 57u, 65u, 90u, 94u, 122u, 0
};

static const char _field_parser_single_lengths[] = {
	0, 3, 3, 1, 2, 2
};

static const char _field_parser_range_lengths[] = {
	0, 6, 1, 0, 1, 6
};

static const char _field_parser_index_offsets[] = {
	0, 0, 10, 15, 17, 21
};

static const char _field_parser_indicies[] = {
	2, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 3, 4, 1, 1, 5, 6, 
	1, 7, 1, 1, 8, 9, 9, 9, 
	9, 9, 9, 9, 9, 1, 0
};

static const char _field_parser_trans_targs[] = {
	1, 0, 2, 3, 2, 4, 5, 3, 
	4, 1
};

static const char _field_parser_trans_actions[] = {
	0, 0, 3, 11, 5, 5, 9, 7, 
	0, 1
};

static const int field_parser_start = 5;
static const int field_parser_first_final = 5;
static const int field_parser_error = 0;

static const int field_parser_en_main = 5;


/* #line 1495 "http_message_parser.cpp" */
	{
	cs = field_parser_start;
	}

/* #line 1498 "http_message_parser.cpp" */
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const unsigned char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _field_parser_trans_keys + _field_parser_key_offsets[cs];
	_trans = _field_parser_index_offsets[cs];

	_klen = _field_parser_single_lengths[cs];
	if ( _klen > 0 ) {
		const unsigned char *_lower = _keys;
		const unsigned char *_mid;
		const unsigned char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _field_parser_range_lengths[cs];
	if ( _klen > 0 ) {
		const unsigned char *_lower = _keys;
		const unsigned char *_mid;
		const unsigned char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _field_parser_indicies[_trans];
	cs = _field_parser_trans_targs[_trans];

	if ( _field_parser_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _field_parser_actions + _field_parser_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
/* #line 81 "rfc2616.rl" */
	{ field_name.iov_base = p; }
	break;
	case 1:
/* #line 81 "rfc2616.rl" */
	{ field_name.iov_len = p - static_cast<char*>(field_name.iov_base); }
	break;
	case 2:
/* #line 81 "rfc2616.rl" */
	{ field_value.iov_base = p; }
	break;
	case 3:
/* #line 81 "rfc2616.rl" */
	{ field_value.iov_len = p - static_cast<char*>(field_value.iov_base); }
	break;
	case 4:
/* #line 256 "http_message_parser.rl" */
	{
        if (
          field_name.iov_len == in_field_name.iov_len
          &&
          memcmp(
            field_name.iov_base,
            in_field_name.iov_base,
            in_field_name.iov_len
          ) == 0
        ) {
          out_field_value = field_value;
          return true;
        }
      }
	break;
/* #line 1599 "http_message_parser.cpp" */
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

/* #line 275 "http_message_parser.rl" */


  return false;
}

void
HttpMessageParser::parse_fields(
  const char* ps,
  const char* pe,
  ::std::vector< std::pair<iovec, iovec> >& fields
) {
  int cs;
  char* p = const_cast<char*>(ps);

  iovec field_name = {0, 0}, field_value = {0, 0};

  // Don't look for the trailing CRLF before the body,
  // since it may not be present yet.
  
/* #line 1628 "http_message_parser.cpp" */
static const char _static_fields_parser_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 2, 2, 3
};

static const char _static_fields_parser_key_offsets[] = {
	0, 0, 15, 20, 21, 25
};

static const unsigned char _static_fields_parser_trans_keys[] = {
	58u, 124u, 126u, 33u, 38u, 42u, 43u, 45u, 
	46u, 48u, 57u, 65u, 90u, 94u, 122u, 13u, 
	32u, 127u, 0u, 31u, 10u, 13u, 127u, 0u, 
	31u, 124u, 126u, 33u, 38u, 42u, 43u, 45u, 
	46u, 48u, 57u, 65u, 90u, 94u, 122u, 0
};

static const char _static_fields_parser_single_lengths[] = {
	0, 3, 3, 1, 2, 2
};

static const char _static_fields_parser_range_lengths[] = {
	0, 6, 1, 0, 1, 6
};

static const char _static_fields_parser_index_offsets[] = {
	0, 0, 10, 15, 17, 21
};

static const char _static_fields_parser_indicies[] = {
	2, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 3, 4, 1, 1, 5, 6, 
	1, 7, 1, 1, 8, 9, 9, 9, 
	9, 9, 9, 9, 9, 1, 0
};

static const char _static_fields_parser_trans_targs[] = {
	1, 0, 2, 3, 2, 4, 5, 3, 
	4, 1
};

static const char _static_fields_parser_trans_actions[] = {
	0, 0, 3, 11, 5, 5, 9, 7, 
	0, 1
};

static const int static_fields_parser_start = 5;
static const int static_fields_parser_first_final = 5;
static const int static_fields_parser_error = 0;

static const int static_fields_parser_en_main = 5;


/* #line 1680 "http_message_parser.cpp" */
	{
	cs = static_fields_parser_start;
	}

/* #line 1683 "http_message_parser.cpp" */
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const unsigned char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _static_fields_parser_trans_keys + _static_fields_parser_key_offsets[cs];
	_trans = _static_fields_parser_index_offsets[cs];

	_klen = _static_fields_parser_single_lengths[cs];
	if ( _klen > 0 ) {
		const unsigned char *_lower = _keys;
		const unsigned char *_mid;
		const unsigned char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _static_fields_parser_range_lengths[cs];
	if ( _klen > 0 ) {
		const unsigned char *_lower = _keys;
		const unsigned char *_mid;
		const unsigned char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _static_fields_parser_indicies[_trans];
	cs = _static_fields_parser_trans_targs[_trans];

	if ( _static_fields_parser_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _static_fields_parser_actions + _static_fields_parser_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
/* #line 81 "rfc2616.rl" */
	{ field_name.iov_base = p; }
	break;
	case 1:
/* #line 81 "rfc2616.rl" */
	{ field_name.iov_len = p - static_cast<char*>(field_name.iov_base); }
	break;
	case 2:
/* #line 81 "rfc2616.rl" */
	{ field_value.iov_base = p; }
	break;
	case 3:
/* #line 81 "rfc2616.rl" */
	{ field_value.iov_len = p - static_cast<char*>(field_value.iov_base); }
	break;
	case 4:
/* #line 300 "http_message_parser.rl" */
	{ fields.push_back(std::make_pair(field_name, field_value)); }
	break;
/* #line 1771 "http_message_parser.cpp" */
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

/* #line 306 "http_message_parser.rl" */

}

bool
HttpMessageParser::parse_fields(
  uint16_t& fields_offset,
  size_t& content_length
) {
  fields_offset = static_cast<uint16_t>(p - ps);

  content_length = 0;

  int cs;
  iovec field_name = {0, 0}, field_value = {0, 0};

  
/* #line 1797 "http_message_parser.cpp" */
static const char _fields_parser_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 2, 
	2, 3, 2, 4, 0
};

static const char _fields_parser_key_offsets[] = {
	0, 0, 15, 16, 31, 36, 37, 52, 
	56
};

static const unsigned char _fields_parser_trans_keys[] = {
	13u, 124u, 126u, 33u, 38u, 42u, 43u, 45u, 
	46u, 48u, 57u, 65u, 90u, 94u, 122u, 10u, 
	58u, 124u, 126u, 33u, 38u, 42u, 43u, 45u, 
	46u, 48u, 57u, 65u, 90u, 94u, 122u, 13u, 
	32u, 127u, 0u, 31u, 10u, 13u, 124u, 126u, 
	33u, 38u, 42u, 43u, 45u, 46u, 48u, 57u, 
	65u, 90u, 94u, 122u, 13u, 127u, 0u, 31u, 
	0
};

static const char _fields_parser_single_lengths[] = {
	0, 3, 1, 3, 3, 1, 3, 2, 
	0
};

static const char _fields_parser_range_lengths[] = {
	0, 6, 0, 6, 1, 0, 6, 1, 
	0
};

static const char _fields_parser_index_offsets[] = {
	0, 0, 10, 12, 22, 27, 29, 39, 
	43
};

static const char _fields_parser_indicies[] = {
	1, 2, 2, 2, 2, 2, 2, 2, 
	2, 0, 3, 0, 6, 4, 4, 4, 
	4, 4, 4, 4, 4, 5, 7, 8, 
	5, 5, 9, 10, 5, 11, 12, 12, 
	12, 12, 12, 12, 12, 12, 0, 13, 
	5, 5, 14, 0, 0
};

static const char _fields_parser_trans_targs[] = {
	0, 2, 3, 8, 3, 0, 4, 5, 
	4, 7, 6, 2, 3, 5, 7
};

static const char _fields_parser_trans_actions[] = {
	13, 0, 1, 11, 0, 0, 3, 15, 
	5, 5, 0, 9, 18, 7, 0
};

static const char _fields_parser_eof_actions[] = {
	0, 13, 13, 0, 0, 0, 13, 0, 
	0
};

static const int fields_parser_start = 1;
static const int fields_parser_first_final = 8;
static const int fields_parser_error = 0;

static const int fields_parser_en_main = 1;


/* #line 1864 "http_message_parser.cpp" */
	{
	cs = fields_parser_start;
	}

/* #line 1867 "http_message_parser.cpp" */
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const unsigned char *_keys;

	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _fields_parser_trans_keys + _fields_parser_key_offsets[cs];
	_trans = _fields_parser_index_offsets[cs];

	_klen = _fields_parser_single_lengths[cs];
	if ( _klen > 0 ) {
		const unsigned char *_lower = _keys;
		const unsigned char *_mid;
		const unsigned char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _fields_parser_range_lengths[cs];
	if ( _klen > 0 ) {
		const unsigned char *_lower = _keys;
		const unsigned char *_mid;
		const unsigned char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _fields_parser_indicies[_trans];
	cs = _fields_parser_trans_targs[_trans];

	if ( _fields_parser_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _fields_parser_actions + _fields_parser_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
/* #line 81 "rfc2616.rl" */
	{ field_name.iov_base = p; }
	break;
	case 1:
/* #line 81 "rfc2616.rl" */
	{ field_name.iov_len = p - static_cast<char*>(field_name.iov_base); }
	break;
	case 2:
/* #line 81 "rfc2616.rl" */
	{ field_value.iov_base = p; }
	break;
	case 3:
/* #line 81 "rfc2616.rl" */
	{ field_value.iov_len = p - static_cast<char*>(field_value.iov_base); }
	break;
	case 4:
/* #line 328 "http_message_parser.rl" */
	{
        parse_content_length_field(
          field_name,
          field_value,
          content_length
        );
      }
	break;
	case 5:
/* #line 336 "http_message_parser.rl" */
	{ {p++; goto _out; } }
	break;
	case 6:
/* #line 337 "http_message_parser.rl" */
	{ return false; }
	break;
/* #line 1965 "http_message_parser.cpp" */
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	p += 1;
	goto _resume;
	if ( p == eof )
	{
	const char *__acts = _fields_parser_actions + _fields_parser_eof_actions[cs];
	unsigned int __nacts = (unsigned int) *__acts++;
	while ( __nacts-- > 0 ) {
		switch ( *__acts++ ) {
	case 6:
/* #line 337 "http_message_parser.rl" */
	{ return false; }
	break;
/* #line 1982 "http_message_parser.cpp" */
		}
	}
	}

	_out: {}
	}

/* #line 342 "http_message_parser.rl" */


  return cs != fields_parser_error;
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
