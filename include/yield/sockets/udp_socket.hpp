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

#ifndef _YIELD_SOCKETS_UDP_SOCKET_HPP_
#define _YIELD_SOCKETS_UDP_SOCKET_HPP_

#include "yield/sockets/datagram_socket.hpp"

namespace yield {
namespace sockets {
/**
  A UDP/IP socket.
*/
class UdpSocket : public DatagramSocket {
public:
  /**
    The default domain of the socket in the (domain, type, protocol) tuple.
    Equivalent to the AF_INET constant on POSIX systems.
  */
  const static int DOMAIN_DEFAULT;

  /**
    The protocol of the socket in the (domain, type, protocol) tuple.
    Equivalent to the IPPROTO_UDP constant on POSIX systems.
  */
  const static int PROTOCOL;

public:
  /**
    Construct a UdpSocket with the given domain.
    @param domain the domain of the new socket
  */
  UdpSocket(int domain = DOMAIN_DEFAULT)
    : DatagramSocket(domain)
  { }

  /**
    Empty virtual destructor.
  */
  virtual ~UdpSocket() { }
};
}
}

#endif
