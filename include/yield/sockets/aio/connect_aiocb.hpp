// yield/sockets/aio/connect_aiocb.hpp

// Copyright (c) 2013 Minor Gordon
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

#include "yield/sockets/aio/aiocb.hpp"

namespace yield {
class Buffer;

namespace sockets {
class StreamSocket;

namespace aio {
/**
  AIO control block for connect operations on sockets.
*/
class connectAIOCB : public AIOCB {
public:
  const static uint32_t TYPE_ID = 2458109810UL;

public:
  /**
    Construct a connectAIOCB with an optional buffer for sending data
      after a connection is established.
    @param socket_ socket to connect
    @param peername address of the peer to connect to
    @param context optional context object
    @param send_buffer optional buffer of data to send after the connection is
      established
  */
  connectAIOCB(
    StreamSocket& socket_,
    SocketAddress& peername,
    Object* context = NULL,
    YO_NEW_REF Buffer* send_buffer = NULL
  );

  ~connectAIOCB();

public:
  /**
    Get the address of the peer to connect to.
    @return the address of the peer to connect to
  */
  const SocketAddress& get_peername() const {
    return peername;
  }

  /**
    Get the buffer of data to send after the connection is established.
    @return the buffer of data to send after the connection is established
  */
  Buffer* get_send_buffer() {
    return send_buffer;
  }

  /**
    Get the socket in this connect operation.
    @return the socket in this connect operation
  */
  StreamSocket& get_socket();

public:
  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }

  const char* get_type_name() const {
    return "yield::sockets::aio::connectAIOCB";
  }

private:
  SocketAddress& peername;
  Buffer* send_buffer;
};

/**
  Print a string representation of a connectAIOCB to a std::ostream.
  @param os std::ostream to print to
  @param connect_aiocb connectAIOCB to print
  @return os
*/
std::ostream& operator<<(std::ostream& os, connectAIOCB& connect_aiocb);
}
}
}

#endif
