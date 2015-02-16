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

#ifndef _YIELD_SOCKETS_AIO_SENDFILE_AIOCB_HPP_
#define _YIELD_SOCKETS_AIO_SENDFILE_AIOCB_HPP_

#include "yield/sockets/aio/socket_aiocb.hpp"

#include <stdio.h>

namespace yield {
namespace sockets {
class StreamSocket;

namespace aio {
/**
  AIO control block for sendfile operations on sockets.
*/
class SendfileAiocb : public SocketAiocb {
public:
  /**
    Construct a SendfileAiocb, passing the same parameters as to sendfile.
    @param socket_ socket to send data on
    @param fd file descriptor from which to send data
    @param context optional context object
  */
  SendfileAiocb(::std::shared_ptr<StreamSocket> socket_, fd_t fd);

  /**
    Construct a SendfileAiocb, passing the same parameters as to sendfile.
    @param socket_ socket to send data on
    @param fd file descriptor from which to send data
    @param offset offset in the file from which to send data
    @param nbytes number of bytes to send
    @param context optional context object
  */
  SendfileAiocb(
    ::std::shared_ptr<StreamSocket> socket_,
    fd_t fd,
    off_t offset,
    size_t nbytes
  );

  virtual ~SendfileAiocb();

public:
  /**
    Get the descriptor of the file from which data is sent.
    @return the descriptor of the file from which data is sent
  */
  fd_t fd() {
    return fd_;
  }

  /**
    Get the number of bytes to send from the file.
    @return the number of bytes to send from the file
  */
  size_t nbytes() const {
    return nbytes_;
  }

  /**
    Get the offset in the file from which to send data.
    @return the offset in the file from which to send data.
  */
  off_t offset() const {
    return offset_;
  }

  /**
    Get the socket in this sendfile operation.
    @return the socket in this sendfile operation
  */
  StreamSocket& socket() override {
    return *socket_;
  }

  Type::Enum type() const override {
    return Type::SENDFILE;
  }

private:
  void init(fd_t fd);

private:
  fd_t fd_;
  size_t nbytes_;
  off_t offset_;
  ::std::shared_ptr<StreamSocket> socket_;
};

/**
  Print a string representation of a SendfileAiocb to a std::ostream.
  @param os std::ostream to print to
  @param sendfile_aiocb SendfileAiocb to print
  @return os
*/
std::ostream& operator<<(std::ostream& os, const SendfileAiocb& sendfile_aiocb);
}
}
}

#endif
