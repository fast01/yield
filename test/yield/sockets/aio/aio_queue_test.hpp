// aio_queue_test.hpp

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

#ifndef _YIELD_SOCKETS_AIO_AIO_QUEUE_TEST_HPP_
#define _YIELD_SOCKETS_AIO_AIO_QUEUE_TEST_HPP_

#include "../../event_queue_test.hpp"
#include "yield/auto_object.hpp"
#include "yield/buffer.hpp"
#include "yield/exception.hpp"
#include "yield/logging.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/stat.hpp"
#include "yield/sockets/datagram_socket_pair.hpp"
#include "yield/sockets/stream_socket_pair.hpp"
#include "yield/sockets/tcp_socket.hpp"
#include "yield/sockets/aio/recv_aiocb.hpp"
#include "yield/sockets/aio/recvfrom_aiocb.hpp"
#include "yield/sockets/aio/send_aiocb.hpp"
#include "yield/sockets/aio/sendfile_aiocb.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace sockets {
namespace aio {
template <class TypeParam>
class AioQueueTest : public ::testing::Test {
public:
void SetUp() {
  TearDown();
  auto_Object<yield::fs::File> file = yield::fs::FileSystem().creat("AioQueueSendFileTest.txt");
  file->write("test", 4);
  file->close();
}

void TearDown() {
  yield::fs::FileSystem().unlink("AioQueueSendFileTest.txt");
}
};

TYPED_TEST_CASE_P(AioQueueTest);

TYPED_TEST_P(AioQueueTest, associate) {
  if (!TypeParam().associate(StreamSocketPair().first())) {
    throw Exception();
  }
}

TYPED_TEST_P(AioQueueTest, recv) {
  TypeParam aio_queue;

  StreamSocketPair sockets;
  if (!aio_queue.associate(sockets.first())) {
    throw Exception();
  }

  sockets.second().send("m", 1, 0);

  auto_Object<Buffer> buffer = new Buffer(4096);
  auto_Object<RecvAiocb> aiocb
  = new RecvAiocb(sockets.first(), buffer->inc_ref(), 0);
  if (!aio_queue.enqueue(aiocb->inc_ref())) {
    throw Exception();
  }

  auto_Object<RecvAiocb> out_aiocb
  = Object::cast<RecvAiocb>(aio_queue.dequeue());
  ASSERT_EQ(&out_aiocb.get(), &aiocb.get());
  ASSERT_EQ(out_aiocb->get_error(), 0);
  ASSERT_EQ(out_aiocb->get_return(), 1);
  ASSERT_EQ(buffer->size(), 1);
  ASSERT_EQ((*buffer)[0], 'm');
}

TYPED_TEST_P(AioQueueTest, recvfrom) {
  TypeParam aio_queue;

  DatagramSocketPair sockets;
  if (!aio_queue.associate(sockets.first())) {
    throw Exception();
  }

  sockets.second().send("m", 1, 0);

  auto_Object<Buffer> buffer = new Buffer(4096);
  auto_Object<RecvfromAiocb> aiocb
  = new RecvfromAiocb(sockets.first(), buffer->inc_ref(), 0);
  if (!aio_queue.enqueue(aiocb->inc_ref())) {
    throw Exception();
  }

  auto_Object<RecvfromAiocb> out_aiocb
  = Object::cast<RecvfromAiocb>(aio_queue.dequeue());
  ASSERT_EQ(&out_aiocb.get(), &aiocb.get());
  ASSERT_EQ(out_aiocb->get_error(), 0);
  ASSERT_EQ(out_aiocb->get_return(), 1);
  ASSERT_EQ(buffer->size(), 1);
  ASSERT_EQ((*buffer)[0], 'm');
}

TYPED_TEST_P(AioQueueTest, recvmsg) {
  TypeParam aio_queue;

  StreamSocketPair sockets;
  if (!aio_queue.associate(sockets.first())) {
    throw Exception();
  }

  sockets.second().send("mm", 2, 0);

  auto_Object<Buffer> buffer = new Buffer(1);
  buffer->set_next_buffer(new Buffer(1));
  auto_Object<RecvAiocb> aiocb
  = new RecvAiocb(sockets.first(), buffer->inc_ref(), 0);
  if (!aio_queue.enqueue(aiocb->inc_ref())) {
    throw Exception();
  }

  auto_Object<RecvAiocb> out_aiocb
  = Object::cast<RecvAiocb>(aio_queue.dequeue());
  ASSERT_EQ(&out_aiocb.get(), &aiocb.get());
  ASSERT_EQ(out_aiocb->get_error(), 0);
  ASSERT_EQ(out_aiocb->get_return(), 2);
  ASSERT_EQ(buffer->size(), 1);
  ASSERT_EQ((*buffer)[0], 'm');
  ASSERT_EQ(buffer->get_next_buffer()->size(), 1);
  ASSERT_EQ((*buffer->get_next_buffer())[0], 'm');
}

TYPED_TEST_P(AioQueueTest, recv_queued) {
  TypeParam aio_queue;

  StreamSocketPair sockets;
  if (!aio_queue.associate(sockets.first())) {
    throw Exception();
  }

  for (uint8_t i = 0; i < 2; i++) {
    RecvAiocb* aiocb = new RecvAiocb(sockets.first(), *new Buffer(2), 0);
    if (!aio_queue.enqueue(*aiocb)) {
      throw Exception();
    }
  }

  // For the NbioQueue: force the first retry
  {
    RecvAiocb* out_aiocb
    = Object::cast<RecvAiocb>(aio_queue.trydequeue());
    ASSERT_EQ(out_aiocb, static_cast<RecvAiocb*>(NULL));
  }

  sockets.second().send("te", 2, 0);

  {
    auto_Object<RecvAiocb> out_aiocb
    = Object::cast<RecvAiocb>(aio_queue.dequeue());
    ASSERT_EQ(out_aiocb->get_error(), 0);
    ASSERT_EQ(out_aiocb->get_return(), 2);
    ASSERT_EQ(out_aiocb->get_buffer(), "te");
    ASSERT_EQ(out_aiocb->get_buffer().size(), 2);
  }

  {
    RecvAiocb* out_aiocb
    = Object::cast<RecvAiocb>(aio_queue.trydequeue());
    ASSERT_EQ(out_aiocb, static_cast<RecvAiocb*>(NULL));
  }

  sockets.second().send("st", 2, 0);

  {
    auto_Object<RecvAiocb> out_aiocb
    = Object::cast<RecvAiocb>(aio_queue.dequeue());
    ASSERT_EQ(out_aiocb->get_error(), 0);
    ASSERT_EQ(out_aiocb->get_return(), 2);
    ASSERT_EQ(out_aiocb->get_buffer(), "st");
    ASSERT_EQ(out_aiocb->get_buffer().size(), 2);
  }
}

TYPED_TEST_P(AioQueueTest, recv_split) {
  TypeParam aio_queue;

  StreamSocketPair sockets;
  if (!aio_queue.associate(sockets.first())) {
    throw Exception();
  }

  for (uint8_t i = 0; i < 2; i++) {
    RecvAiocb* aiocb = new RecvAiocb(sockets.first(), *new Buffer(2), 0);
    if (!aio_queue.enqueue(*aiocb)) {
      throw Exception();
    }
  }

  // For the NbioQueue: force the first retry
  {
    RecvAiocb* out_aiocb
    = Object::cast<RecvAiocb>(aio_queue.trydequeue());
    ASSERT_EQ(out_aiocb, static_cast<RecvAiocb*>(NULL));
  }

  sockets.second().send("test", 4, 0);

  for (uint8_t i = 0; i < 2; i++) {
    auto_Object<RecvAiocb> out_aiocb
    = Object::cast<RecvAiocb>(aio_queue.dequeue());
    ASSERT_EQ(out_aiocb->get_error(), 0);
    ASSERT_EQ(out_aiocb->get_return(), 2);
    ASSERT_EQ(out_aiocb->get_buffer(), i == 0 ? "te" : "st");
    ASSERT_EQ(out_aiocb->get_buffer().size(), 2);
  }
}

TYPED_TEST_P(AioQueueTest, send) {
  TypeParam aio_queue;

  StreamSocketPair sockets;
  if (!aio_queue.associate(sockets.first())) {
    throw Exception();
  }

  auto_Object<SendAiocb> aiocb
  = new SendAiocb(sockets.first(), Buffer::copy("test"), 0);
  if (!aio_queue.enqueue(aiocb->inc_ref())) {
    throw Exception();
  }

  auto_Object<SendAiocb> out_aiocb
  = Object::cast<SendAiocb>(aio_queue.dequeue());
  ASSERT_EQ(&out_aiocb.get(), &aiocb.get());
  ASSERT_EQ(out_aiocb->get_error(), 0);
  ASSERT_EQ(out_aiocb->get_return(), 4);

  char test[4];
  ssize_t recv_ret = sockets.second().recv(test, 4, 0);
  ASSERT_EQ(recv_ret, 4);
  ASSERT_EQ(memcmp(test, "test", 4), 0);
}

TYPED_TEST_P(AioQueueTest, sendfile) {
  TypeParam aio_queue;

  StreamSocketPair sockets;
  if (!aio_queue.associate(sockets.first())) {
    throw Exception();
  }

  auto_Object<yield::fs::File> file
  = yield::fs::FileSystem().open("AioQueueSendFileTest.txt");
  auto_Object<yield::fs::Stat> stbuf = file->stat();

  auto_Object<SendfileAiocb> aiocb
  = new SendfileAiocb(sockets.first(), *file);
  ASSERT_EQ(aiocb->get_nbytes(), stbuf->get_size());
  ASSERT_EQ(aiocb->get_offset(), 0);

  if (!aio_queue.enqueue(aiocb->inc_ref())) {
    throw Exception();
  }

  auto_Object<SendfileAiocb> out_aiocb
  = Object::cast<SendfileAiocb>(aio_queue.dequeue());
  ASSERT_EQ(&out_aiocb.get(), &aiocb.get());
  ASSERT_EQ(out_aiocb->get_error(), 0);
  ASSERT_EQ(
    out_aiocb->get_return(),
    static_cast<ssize_t>(stbuf->get_size())
  );

  char test[4];
  ssize_t recv_ret = sockets.second().recv(test, 4, 0);
  ASSERT_EQ(recv_ret, 4);
  ASSERT_EQ(memcmp(test, "test", 4), 0);
}

TYPED_TEST_P(AioQueueTest, sendmsg) {
  TypeParam aio_queue;

  StreamSocketPair sockets;
  if (!aio_queue.associate(sockets.first())) {
    throw Exception();
  }

  auto_Object<Buffer> buffer = Buffer::copy("test");
  buffer->set_next_buffer(Buffer::copy(" string"));
  auto_Object<SendAiocb> aiocb
  = new SendAiocb(sockets.first(), buffer->inc_ref(), 0);
  if (!aio_queue.enqueue(aiocb->inc_ref())) {
    throw Exception();
  }

  auto_Object<SendAiocb> out_aiocb
  = Object::cast<SendAiocb>(aio_queue.dequeue());
  ASSERT_EQ(&out_aiocb.get(), &aiocb.get());
  ASSERT_EQ(out_aiocb->get_error(), 0);
  ASSERT_EQ(out_aiocb->get_return(), 11);

  char test_string[11];
  ssize_t recv_ret = sockets.second().recv(test_string, 11, 0);
  ASSERT_EQ(recv_ret, 11);
  ASSERT_EQ(memcmp(test_string, "test string", 11), 0);
}

REGISTER_TYPED_TEST_CASE_P(AioQueueTest, associate, recv, recvfrom, recvmsg, recv_queued, recv_split, send, sendmsg, sendfile);
}
}
}

#endif
