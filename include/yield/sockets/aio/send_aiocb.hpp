// yield/sockets/aio/send_aiocb.hpp

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

#ifndef _YIELD_SOCKETS_AIO_SEND_AIOCB_HPP_
#define _YIELD_SOCKETS_AIO_SEND_AIOCB_HPP_

#include "yield/sockets/aio/aiocb.hpp"
#include "yield/sockets/socket.hpp"

namespace yield {
namespace sockets {
namespace aio {
/**
  AIO control block for send operations on sockets.
*/
class SendAiocb : public Aiocb {
public:
  /**
    Construct a SendAiocb, passing the same parameters as to send.
    @param socket_ socket to send data on
    @param buffer buffer to send data from
    @param flags flags to pass to the send method
    @param context optional context object
  */
  SendAiocb(
    Socket& socket_,
    YO_NEW_REF Buffer& buffer,
    const Socket::MessageFlags& flags,
    Object* context = NULL
  ) : Aiocb(socket_, context),
    buffer(buffer),
    flags(flags) {
  }

  ~SendAiocb();

public:
  /**
    Get the buffer from which to send data.
    @return the buffer from which to send data
  */
  Buffer& get_buffer() const {
    return buffer;
  }

  /**
    Get the flags to pass to the send method.
    @return the flags to pass to the send method
  */
  const Socket::MessageFlags& get_flags() const {
    return flags;
  }

private:
  Buffer& buffer;
  Socket::MessageFlags flags;
  SocketAddress* peername;
};

/**
  Print a string representation of a SendAiocb to a std::ostream.
  @param os std::ostream to print to
  @param send_aiocb SendAiocb to print
  @return os
*/
std::ostream& operator<<(std::ostream& os, const SendAiocb& send_aiocb);
}
}
}

#endif
