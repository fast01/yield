// yield/sockets/aio/accept_aiocb.cpp

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

#include "yield/buffer.hpp"
#include "yield/logging.hpp"
#include "yield/sockets/stream_socket.hpp"
#include "yield/sockets/aio/accept_aiocb.hpp"

namespace yield {
namespace sockets {
namespace aio {
AcceptAiocb::AcceptAiocb(
  StreamSocket& socket_,
  YO_NEW_REF Buffer* recv_buffer
)
  : Aiocb(socket_, context),
    recv_buffer(recv_buffer) {
  accepted_socket = NULL;
  peername = NULL;
  if (recv_buffer != NULL) {
    CHECK_EQ(recv_buffer->get_next_buffer(), NULL);
  }
}

AcceptAiocb::~AcceptAiocb() {
  StreamSocket::dec_ref(accepted_socket);
  SocketAddress::dec_ref(peername);
  Buffer::dec_ref(recv_buffer);
}

StreamSocket& AcceptAiocb::get_socket() {
  return static_cast<StreamSocket&>(Aiocb::get_socket());
}

void
AcceptAiocb::set_accepted_socket(
  YO_NEW_REF StreamSocket& accepted_socket
) {
  StreamSocket::dec_ref(this->accepted_socket);
  this->accepted_socket = &accepted_socket;
}

void AcceptAiocb::set_peername(YO_NEW_REF SocketAddress* peername) {
  SocketAddress::dec_ref(this->peername);
  this->peername = peername;
}

void AcceptAiocb::set_recv_buffer(YO_NEW_REF Buffer* recv_buffer) {
  Buffer::dec_ref(this->recv_buffer);
  this->recv_buffer = recv_buffer;
}

std::ostream& operator<<(std::ostream& os, const AcceptAiocb& accept_aiocb) {
  os <<
     "AcceptAiocb(" <<
     //"accepted_socket=" << accept_aiocb.get_accepted_socket() <<
     //", " <<
     //"error=" << accept_aiocb.get_error() <<
     //", " <<
     //"peername=" << accept_aiocb.get_peername() <<
     //", " <<
     //"recv_buffer=" << accept_aiocb.get_recv_buffer() <<
     //", " <<
     "return=" << accept_aiocb.get_return() <<
     //", " <<
     //"socket=" << accept_aiocb.get_socket() <<
     ")";
  return os;
}
}
}
}
