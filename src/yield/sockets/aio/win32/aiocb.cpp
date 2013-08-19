// yield/sockets/aio/win32/aiocb.cpp

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

#include "yield/sockets/socket.hpp"
#include "yield/sockets/aio/aiocb.hpp"

#include <Windows.h>

namespace yield {
namespace sockets {
namespace aio {
Aiocb::Aiocb(Socket& socket_, Object* context)
  : context(Object::inc_ref(context)),
    socket_(socket_.inc_ref()) {
  static_assert(sizeof(overlapped) == sizeof(::OVERLAPPED), "");
  memset(&overlapped, 0, sizeof(overlapped));
  this_ = this;

  error = 0;
  return_ = -1;
}

Aiocb::Aiocb(Socket& socket_, off_t offset, Object *context)
  : context(Object::inc_ref(context)),
    socket_(socket_.inc_ref()) {
  memset(&overlapped, 0, sizeof(overlapped));
  overlapped.Offset = static_cast<uint32_t>(offset);
  overlapped.OffsetHigh = static_cast<uint32_t>(offset >> 32);
  this_ = this;

  error = 0;
  return_ = -1;
}

Aiocb::~Aiocb() {
  Object::dec_ref(context);
  Socket::dec_ref(socket_);
}

Aiocb& Aiocb::cast(::OVERLAPPED& lpOverlapped) {
  Aiocb* aiocb;

  memcpy_s(
    &aiocb,
    sizeof(aiocb),
    reinterpret_cast<char*>(&lpOverlapped) + sizeof(::OVERLAPPED),
    sizeof(aiocb)
  );

  return *aiocb;
}

Aiocb::operator ::OVERLAPPED* () {
  return reinterpret_cast<::OVERLAPPED*>(&overlapped);
}
}
}
}
