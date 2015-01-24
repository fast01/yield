// yield/sockets/aio/accept_aiocb.hpp

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

#ifndef _YIELD_SOCKETS_AIO_ACCEPT_AIOCB_HPP_
#define _YIELD_SOCKETS_AIO_ACCEPT_AIOCB_HPP_

#include <ostream>

#include "yield/sockets/aio/aiocb.hpp"

namespace yield {
class Buffer;

namespace sockets {
class Socket;
class SocketAddress;
class StreamSocket;

namespace aio {
/**
  AIO control block for accept operations on sockets.
*/
class AcceptAiocb : public Aiocb {
public:
  /**
    Construct an AcceptAiocb with an optional buffer for receiving data
      after a new connection is accepted.
    @param socket_ listen socket to accept on
    @param recv_buffer optional buffer for receiving data after a new
      connection is accepted
  */
  AcceptAiocb(
    ::std::shared_ptr<StreamSocket> socket_,
    ::std::shared_ptr<Buffer> recv_buffer
  ) : Aiocb(socket_),
      recv_buffer_(recv_buffer) {
  }

public:
  /**
    Get the accepted socket, once the accept operation is complete.
    @return the accepted socket
  */
  ::std::shared_ptr<StreamSocket> accepted_socket() {
    return accepted_socket_;
  }

  /**
    Get the address of the peer of the accepted connection, once the accept
      operation is complete.
    @return the new peer's address
  */
  ::std::shared_ptr<SocketAddress> peername() {
    return peername_;
  }

  /**
    Get the buffer with data received from the new peer.
    The buffer is updated after the accept operation if it is specified.
    @return the buffer with data received from the new peer, or NULL
      if no buffer was passed to the constructor
  */
  ::std::shared_ptr<Buffer> recv_buffer() {
    return recv_buffer_;
  }

  /**
    Get the listen socket in this accept operation.
  */
  StreamSocket& socket() {
    return static_cast<StreamSocket&>(Aiocb::socket());
  }

  Type::Enum type() const {
    return Type::ACCEPT;
  }

protected:
#ifdef _WIN32
  friend class AioQueue;
#endif
  friend class NbioQueue;

  void set_accepted_socket(::std::shared_ptr<StreamSocket> accepted_socket) {
    this->accepted_socket_ = accepted_socket;
  }

  void set_peername(::std::shared_ptr<SocketAddress> peername) {
    this->peername_ = peername;
  }

  void set_recv_buffer(::std::shared_ptr<Buffer> recv_buffer) {
    this->recv_buffer_ = recv_buffer;
  }

private:
  ::std::shared_ptr<StreamSocket> accepted_socket_;
  ::std::shared_ptr<SocketAddress> peername_;
  ::std::shared_ptr<Buffer> recv_buffer_;
};

/**
  Print a string representation of an AcceptAiocb to a std::ostream.
  @param os std::ostream to print to
  @param accept_aiocb AcceptAiocb to print
  @return os
*/
::std::ostream& operator<<(::std::ostream& os, const AcceptAiocb& accept_aiocb);
}
}
}

#endif
