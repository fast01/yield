// partial_send_stream_socket.hpp

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

#ifndef _YIELD_SOCKETS_AIO_PARTIAL_SEND_STREAM_SOCKET_HPP_
#define _YIELD_SOCKETS_AIO_PARTIAL_SEND_STREAM_SOCKET_HPP_

#include "yield/sockets/stream_socket.hpp"

namespace yield {
namespace sockets {
namespace aio {
class PartialSendStreamSocket final : public StreamSocket {
public:
  PartialSendStreamSocket(::std::shared_ptr<StreamSocket> stream_socket)
    : StreamSocket(
      stream_socket->domain(),
      stream_socket->protocol(),
      static_cast<socket_t>(-1)
    ),
    stream_socket_(stream_socket) {
  }

public:
  // yield::sockets::Socket
  operator socket_t() const override {
    return stream_socket_->operator socket_t();
  }

  ssize_t send(const void* buf, size_t buflen, const MessageFlags& flags) override {
    return stream_socket_->send(buf, 1, flags);
  }

  ssize_t
  sendmsg(
    const iovec* iov,
    int iovlen,
    const MessageFlags& flags
  ) override {
    CHECK_GT(iovlen, 0);
    iovec iov0 = iov[0];
    iov0.iov_len = 1;
    return stream_socket_->sendmsg(&iov0, 1, flags);
  }

  bool set_blocking_mode(bool blocking_mode) override {
    return stream_socket_->set_blocking_mode(blocking_mode);
  }

public:
  // yield::sockets::StreamSocket
  ssize_t sendfile(fd_t fd, off_t offset, size_t nbytes) override {
    return stream_socket_->sendfile(fd, offset, 1);
  }

private:
  ::std::shared_ptr<StreamSocket> stream_socket_;
};
}
}
}

#endif
