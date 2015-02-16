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

#ifndef _YIELD_HTTP_HTTP_MESSAGE_BODY_CHUNK_HPP_
#define _YIELD_HTTP_HTTP_MESSAGE_BODY_CHUNK_HPP_

#include "yield/buffer.hpp"

namespace yield {
namespace http {
/**
  An RFC 2616 HTTP message body chunk.
  Used for sending streams of chunked bodies (incoming as well as outgoing)
    as separate Events from the HttpRequest or HttpResponse header.
*/
class HttpMessageBodyChunk {
public:
  /**
    Construct an HttpMessageBodyChunk with the given data.
    Steals the reference to data.
  */
  HttpMessageBodyChunk(::std::shared_ptr<Buffer> data)
    : data_(data)
  { }

  /**
    Destruct an HttpMessageBodyChunk, releasing its underlying buffer.
  */
  virtual ~HttpMessageBodyChunk() {
  }

public:
  /**
    Get the chunk data.
    @return the chunk data
  */
  ::std::shared_ptr<Buffer>& data() {
    return data_;
  }

  /**
    Check if this chunk is the last in the HTTP message body i.e., the
    terminating 0CRLFCRLF chunk with no data.
  */
  bool is_last() const {
    return data_ == NULL;
  }

  /**
    Get the size of the chunk data.
    @return the size of the chunk data
  */
  size_t size() const {
    return data_ != NULL ? data_->size() : 0;
  }

private:
  ::std::shared_ptr<Buffer> data_;
};
}
}

#endif
