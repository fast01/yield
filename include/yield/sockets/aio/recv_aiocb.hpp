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

#ifndef _YIELD_SOCKETS_AIO_RECV_AIOCB_HPP_
#define _YIELD_SOCKETS_AIO_RECV_AIOCB_HPP_

#include "yield/sockets/socket.hpp"
#include "yield/sockets/aio/socket_aiocb.hpp"

namespace yield {
namespace sockets {
namespace aio {
/**
  AIO control block for receive operations on sockets.
*/
class RecvAiocb : public SocketAiocb {
public:
  /**
    Construct a RecvAiocb, passing the same parameters as to recv.
    @param socket_ socket to receive data on
    @param buffer buffer to receive data into
    @param flags flags to pass to the recv method
    @param context optional context object
  */
  RecvAiocb(
    ::std::shared_ptr<Socket> socket_,
    ::std::shared_ptr<Buffer> buffer,
    const Socket::MessageFlags& flags
  ) : buffer_(buffer),
    flags_(flags),
    socket_(socket_)
  { }

  virtual ~RecvAiocb() {
  }

public:
  /**
    Get the buffer in which to receive data.
    @return the buffer in which to receive data
  */
  const Buffer& buffer() const {
    return *buffer_;
  }

  /**
    Get the buffer in which to receive data.
    @return the buffer in which to receive data
  */
  ::std::shared_ptr<Buffer>& buffer() {
    return buffer_;
  }

  /**
    Get the flags to pass to the recv method.
    @return the flags to pass to the recv method
  */
  const Socket::MessageFlags& flags() const {
    return flags_;
  }

  /**
    Get the socket associated with this control block.
  */
  Socket& socket() override {
    return *socket_;
  }

  Type::Enum type() const override {
    return Type::RECV;
  }

private:
  ::std::shared_ptr<Buffer> buffer_;
  Socket::MessageFlags flags_;
  ::std::shared_ptr<Socket> socket_;
};

/**
  Print a string representation of a RecvAiocb to a std::ostream.
  @param os std::ostream to print to
  @param recv_aiocb RecvAiocb to print
  @return os
*/
std::ostream& operator<<(std::ostream& os, const RecvAiocb& recv_aiocb);
}
}
}

#endif
