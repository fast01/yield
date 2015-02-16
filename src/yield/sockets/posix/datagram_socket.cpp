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

#include "yield/sockets/datagram_socket.hpp"

#include <sys/socket.h>

namespace yield {
namespace sockets {
const int DatagramSocket::TYPE = SOCK_DGRAM;

ssize_t
DatagramSocket::sendmsg(
  const iovec* iov,
  int iovlen,
  const MessageFlags& flags,
  const SocketAddress& peername
) {
  msghdr msghdr_;
  memset(&msghdr_, 0, sizeof(msghdr_));
  msghdr_.msg_iov = const_cast<iovec*>(iov);
  msghdr_.msg_iovlen = iovlen;
  const SocketAddress* peername_ = peername.filter(domain());
  if (peername_ != NULL) {
    const sockaddr* peername_sockaddr = *peername_;
    msghdr_.msg_name = const_cast<sockaddr*>(peername_sockaddr);
    msghdr_.msg_namelen = peername_->len();
  } else {
    return -1;
  }
  return ::sendmsg(*this, &msghdr_, flags);
}

ssize_t
DatagramSocket::sendto(
  const void* buf,
  size_t buflen,
  const MessageFlags& flags,
  const SocketAddress& peername
) {
  const SocketAddress* peername_ = peername.filter(domain());
  if (peername_ != NULL) {
    return ::sendto(
             *this,
             static_cast<const char*>(buf),
             buflen,
             flags,
             *peername_,
             peername_->len()
           );
  } else {
    return -1;
  }
}
}
}
