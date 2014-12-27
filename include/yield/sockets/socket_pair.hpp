// yield/sockets/socket_pair.hpp

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

#ifndef _YIELD_SOCKETS_SOCKET_PAIR_HPP_
#define _YIELD_SOCKETS_SOCKET_PAIR_HPP_

#include <memory>

#include "yield/channel_pair.hpp"

namespace yield {
namespace sockets {
/**
  A pair of connected sockets. The (domain, type, protocol) of the sockets
    are platform-specific, because not all combinations are supported on
    every platform.
  The sockets in the pair are functionally equivalent: they are both connected
    to each other at full-duplex.
  Equivalent to socketpair(2) sockets on POSIX systems.
*/
template <class SocketType>
class SocketPair : public ChannelPair {
public:
  /**
    Destruct the SocketPair, releasing underlying system resources as necessary.
  */
  virtual ~SocketPair() {
  }

public:
  /**
    Get the "first" socket in the pair.
    Since the two sockets are functionally equivalent this is only a convention.
    @return the "first" socket in the pair
  */
  SocketType& first() {
    return *first_;
  }

  /**
    Get the "second" socket in the pair.
    Since the two sockets are functionally equivalent this is only a convention.
    @return the "second" socket in the pair
  */
  SocketType& second() {
    return *second_;
  }

public:
  // yield::ChannelPair
  Channel& get_read_channel() {
    return first();
  }

  Channel& get_write_channel() {
    return second();
  }

protected:
  SocketPair() {
  }

protected:
  ::std::unique_ptr<SocketType> first_;
  ::std::unique_ptr<SocketType> second_;
};
}
}

#endif
