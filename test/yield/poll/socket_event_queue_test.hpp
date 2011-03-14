// socket_event_queue_test.hpp

// Copyright (c) 2011 Minor Gordon
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

#ifndef _TEST_YIELD_POLL_SOCKET_EVENT_QUEUE_TEST_HPP_
#define _TEST_YIELD_POLL_SOCKET_EVENT_QUEUE_TEST_HPP_

#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/exception.hpp"
#include "yield/poll/socket_event.hpp"
#include "yield/sockets/socket_pair.hpp"
#include "yield/poll/socket_event_queue.hpp"
#include "yunit.hpp"


namespace yield {
namespace poll {
template <class SocketEventQueueType>
class SocketEventQueueTest : public yunit::Test {
public:
  SocketEventQueueTest() {
    socket_event_queue = NULL;
    sockets = NULL;
  }

  // yunit::Test
  void setup() {
    socket_event_queue = new SocketEventQueueType;
    sockets = new yield::sockets::SocketPair;
  }

  void teardown() {
    delete socket_event_queue;
    delete sockets;
  }

protected:
  SocketEventQueueType& get_socket_event_queue() const {
    return *socket_event_queue;
  }
  socket_t get_read_socket() const {
    return sockets->first();
  }
  socket_t get_write_socket() const {
    return sockets->second();
  }

private:
  SocketEventQueueType* socket_event_queue;
  yield::sockets::SocketPair* sockets;
};


template <class SocketEventQueueType>
class SocketEventQueueAssociateOneTest
    : public SocketEventQueueTest<SocketEventQueueType> {
public:
  void run() {
    if (!this->get_socket_event_queue().associate(get_read_socket(), POLLIN))
      throw Exception();
  }
};


template <class SocketEventQueueType>
class SocketEventQueueAssociateTwoTest
    : public SocketEventQueueTest<SocketEventQueueType> {
public:
  void run() {
    if (!get_socket_event_queue().associate(get_read_socket(), POLLIN))
      throw Exception();

    if (!get_socket_event_queue().associate(get_write_socket(), POLLOUT))
      throw Exception();
  }
};


template <class SocketEventQueueType>
class SocketEventQueueDissociateOneTest
    : public SocketEventQueueTest<SocketEventQueueType> {
public:
  void run() {
    if (!get_socket_event_queue().associate(get_read_socket(), POLLIN))
      throw Exception();

    if (!get_socket_event_queue().dissociate(get_read_socket()))
      throw Exception();

    send(get_read_socket(), "m", 1, 0);

    Event* event = get_socket_event_queue().trydequeue();
    throw_assert_eq(event, NULL);

    if (!get_socket_event_queue().associate(get_read_socket(), POLLIN))
      throw Exception(); // associate after dissociate should succeed
  }
};


template <class SocketEventQueueType>
class SocketEventQueueDissociateTwoTest
    : public SocketEventQueueTest<SocketEventQueueType> {
public:
  void run() {
    if (!get_socket_event_queue().associate(get_read_socket(), POLLIN))
      throw Exception();
    if (!get_socket_event_queue().associate(get_write_socket(), POLLIN))
      throw Exception();

    if (!get_socket_event_queue().dissociate(get_read_socket()))
      throw Exception();

    send(get_read_socket(), "m", 1, 0);
    send(get_write_socket(), "m", 1, 0);

    auto_Object<SocketEvent> socket_event
    = object_cast<SocketEvent>(get_socket_event_queue().dequeue());
    throw_assert_eq(socket_event->get_socket(), get_write_socket());
    throw_assert_ne(*socket_event & POLLIN, 0);

    Event* event = get_socket_event_queue().trydequeue();
    throw_assert_eq(event, NULL);
  }
};


template <class SocketEventQueueType>
class SocketEventQueuePollOneBlockingTest
    : public SocketEventQueueTest<SocketEventQueueType> {
public:
  void run() {
    if (!get_socket_event_queue().associate(get_read_socket(), POLLIN))
      throw Exception();

    send(get_write_socket(), "m", 1, 0);

    auto_Object<SocketEvent> socket_event
    = object_cast<SocketEvent>(get_socket_event_queue().dequeue());
  }
};


template <class SocketEventQueueType>
class SocketEventQueuePollOneTimedTest
    : public SocketEventQueueTest<SocketEventQueueType> {
public:
  void run() {
    if (!get_socket_event_queue().associate(get_read_socket(), POLLIN))
      throw Exception();

    send(get_write_socket(), "m", 1, 0);

    Time start_time(Time::now());
    auto_Object<SocketEvent> socket_event
    = object_cast<SocketEvent>(get_socket_event_queue().dequeue(10.0));
    Time elapsed_time(Time::now() - start_time);
    throw_assert_le(elapsed_time, Time(10.0));
  }
};


template <class SocketEventQueueType>
class SocketEventQueuePollTwoTest
    : public SocketEventQueueTest<SocketEventQueueType> {
public:
  void run() {
    if (!get_socket_event_queue().associate(get_read_socket(), POLLIN))
      throw Exception();
    if (!get_socket_event_queue().associate(get_write_socket(), POLLIN))
      throw Exception();

    send(get_read_socket(), "m", 1, 0);
    send(get_write_socket(), "m", 1, 0);

    for (uint8_t socket_i = 0; socket_i < 2; socket_i++) {
      auto_Object<SocketEvent> socket_event
      = object_cast<SocketEvent>(get_socket_event_queue().dequeue());

      throw_assert
      (
        socket_event->get_socket() == get_read_socket()
        ||
        socket_event->get_socket() == get_write_socket()
      );
      throw_assert_ne(*socket_event & POLLIN, 0);
    }
  }
};


template <class SocketEventQueueType>
class SocketEventQueueToggleOneTest
    : public SocketEventQueueTest<SocketEventQueueType> {
public:
  void run() {
    if (!get_socket_event_queue().associate(get_read_socket(), POLLIN | POLLOUT))
      throw Exception();

    if (!get_socket_event_queue().associate(get_read_socket(), POLLIN))
      throw Exception();

    send(get_write_socket(), "m", 1, 0);

    auto_Object<SocketEvent> socket_event
    = object_cast<SocketEvent>(get_socket_event_queue().dequeue());
  }
};


template <class SocketEventQueueType>
class SocketEventQueueWantSendTest
    : public SocketEventQueueTest<SocketEventQueueType> {
public:
  void run() {
    if (!get_socket_event_queue().associate(get_read_socket(), POLLOUT))
      throw Exception();

    auto_Object<SocketEvent> socket_event
    = object_cast<SocketEvent>(get_socket_event_queue().dequeue());
    throw_assert_eq(socket_event->get_socket(), get_read_socket());
    throw_assert_ne(*socket_event & POLLOUT, 0);
  }
};


template <class SocketEventQueueType>
class SocketEventQueueTestSuite : public yunit::TestSuite {
public:
  SocketEventQueueTestSuite() {
    add
    (
      "SocketEventQueue::associate( socket )",
      new SocketEventQueueAssociateOneTest<SocketEventQueueType>
    );

    add
    (
      "SocketEventQueue::associate( socket ) x 2",
      new SocketEventQueueAssociateTwoTest<SocketEventQueueType>
    );

    add
    (
      "SocketEventQueue::dissociate( socket )",
      new SocketEventQueueDissociateOneTest<SocketEventQueueType>
    );

    add
    (
      "SocketEventQueue::dissociate( socket ) x 2",
      new SocketEventQueueDissociateTwoTest<SocketEventQueueType>
    );

    add
    (
      "SocketEventQueue::poll()",
      new SocketEventQueuePollOneBlockingTest<SocketEventQueueType>
    );

    add
    (
      "SocketEventQueue::poll( timeout )",
      new SocketEventQueuePollOneTimedTest<SocketEventQueueType>
    );

    add
    (
      "SocketEventQueue::poll() x 2",
      new SocketEventQueuePollTwoTest<SocketEventQueueType>
    );

    add
    (
      "SocketEventQueue::toggle",
      new SocketEventQueueToggleOneTest<SocketEventQueueType>
    );

    add
    (
      "SocketEventQueue::want_send",
      new SocketEventQueueWantSendTest<SocketEventQueueType>
    );
  }
};
}
}

#endif