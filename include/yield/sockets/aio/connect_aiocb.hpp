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

#ifndef _YIELD_SOCKETS_AIO_CONNECT_AIOCB_HPP_
#define _YIELD_SOCKETS_AIO_CONNECT_AIOCB_HPP_

#include "yield/sockets/aio/socket_aiocb.hpp"

namespace yield {
class Buffer;

namespace sockets {
class StreamSocket;

namespace aio {
/**
  AIO control block for connect operations on sockets.
*/
class ConnectAiocb : public SocketAiocb {
public:
  /**
    Construct a ConnectAiocb with an optional buffer for sending data
      after a connection is established.
    @param socket_ socket to connect
    @param peername address of the peer to connect to
    @param context optional context object
    @param send_buffer optional buffer of data to send after the connection is
      established
  */
  ConnectAiocb(
    ::std::shared_ptr<StreamSocket> socket_,
    ::std::shared_ptr<SocketAddress> peername,
    ::std::shared_ptr<Buffer> send_buffer
  );

  virtual ~ConnectAiocb() {
  }

public:
  /**
    Get the address of the peer to connect to.
    @return the address of the peer to connect to
  */
  const SocketAddress& peername() const {
    return *peername_;
  }

  /**
    Get the buffer of data to send after the connection is established.
    @return the buffer of data to send after the connection is established
  */
  Buffer* send_buffer() {
    return send_buffer_.get();
  }

  /**
    Get the socket in this connect operation.
    @return the socket in this connect operation
  */
  StreamSocket& socket() override {
    return *socket_;
  }

  Type::Enum type() const override {
    return Type::CONNECT;
  }

private:
  ::std::shared_ptr<SocketAddress> peername_;
  ::std::shared_ptr<Buffer> send_buffer_;
  ::std::shared_ptr<StreamSocket> socket_;
};

/**
  Print a string representation of a ConnectAiocb to a std::ostream.
  @param os std::ostream to print to
  @param connect_aiocb ConnectAiocb to print
  @return os
*/
std::ostream& operator<<(std::ostream& os, const ConnectAiocb& connect_aiocb);
}
}
}

#endif
