// yield/fs/aio/pread_aiocb.cpp

// Copyright (c) 2011 Minor Gordon
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

#include "yield/exception.hpp"
#include "yield/buffer.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/aio/pread_aiocb.hpp"

namespace yield {
namespace fs {
namespace aio {
preadAIOCB::preadAIOCB(
  File& file,
  YO_NEW_REF Buffer& buffer,
  uint64_t offset
) : AIOCB(file, buffer, buffer.capacity() - buffer.size(), offset),
    buffer(buffer)
{ }

preadAIOCB::~preadAIOCB() {
  Buffer::dec_ref(buffer);
}

void preadAIOCB::set_return(ssize_t return_) {
  if (return_ >= 0) {
    Buffer* buffer = &get_buffer();
    for (;;) {
      size_t page_left = buffer->capacity() - buffer->size();

      if (static_cast<size_t>(return_) <= page_left) {
        buffer->resize(buffer->size() + return_);
        break;
      } else {
        buffer->resize(buffer->capacity());
        return_ -= page_left;
        buffer = buffer->get_next_buffer();
      }
    }
  }

  AIOCB::set_return(return_);
}

preadAIOCB::RetryStatus preadAIOCB::retry() {
  ssize_t return_;

  if (get_buffer().get_next_buffer() == NULL) {   // pread
    return_
    = get_file().pread(
        static_cast<char*>(get_buffer()) + get_buffer().size(),
        get_nbytes(),
        get_offset()
      );
  } else { // preadv
    vector<iovec> iov;
    Buffer* buffer = &get_buffer();
    do {
      iovec page_iov;
      page_iov.iov_base = static_cast<char*>(*buffer) + buffer->size();
      page_iov.iov_len = buffer->capacity() - buffer->size();
      iov.push_back(page_iov);
      buffer = buffer->get_next_buffer();
    } while (buffer != NULL);

    return_
    = get_file().preadv
      (
        &iov[0],
        iov.size(),
        get_offset()
      );
  }

  if (return_ >= 0) {
    set_return(return_);
    return RETRY_STATUS_COMPLETE;
  } else {
    set_error(Exception::get_last_error_code());
    return RETRY_STATUS_ERROR;
  }
}
}
}
}
