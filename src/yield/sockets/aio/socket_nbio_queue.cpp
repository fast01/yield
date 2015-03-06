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

#include "yield/buffer.hpp"
#include "yield/exception.hpp"
#include "yield/logging.hpp"
#include "yield/time.hpp"
#include "yield/sockets/stream_socket.hpp"
#include "yield/sockets/aio/accept_aiocb.hpp"
#include "yield/sockets/aio/socket_nbio_queue.hpp"
#include "yield/sockets/aio/connect_aiocb.hpp"
#include "yield/sockets/aio/recv_aiocb.hpp"
#include "yield/sockets/aio/recvfrom_aiocb.hpp"
#include "yield/sockets/aio/send_aiocb.hpp"
#include "yield/sockets/aio/sendfile_aiocb.hpp"

namespace yield {
namespace sockets {
namespace aio {
using ::std::map;
using ::std::move;
using ::std::shared_ptr;
using ::std::unique_ptr;
using ::std::vector;
using ::yield::poll::FdEvent;

class SocketNbioQueue::AiocbState {
public:
  AiocbState(unique_ptr<SocketAiocb> aiocb, ssize_t partial_send_len)
    : aiocb_(move(aiocb)),
      partial_send_len_(partial_send_len) {
    next_aiocb_state_ = NULL;
  }

  ~AiocbState() {
    delete next_aiocb_state_;
  }

public:
  unique_ptr<SocketAiocb> aiocb_;
  AiocbState* next_aiocb_state_;
  size_t partial_send_len_;
};

class SocketNbioQueue::SocketState {
public:
  SocketState() {
    memset(aiocb_state, 0, sizeof(aiocb_state));
  }

  ~SocketState() {
    for (uint8_t i = 0; i < 4; ++i) {
      delete aiocb_state[i];
    }
  }

public:
  bool empty() const {
    for (uint8_t i = 0; i < 4; ++i) {
      if (aiocb_state[i] != NULL) {
        return false;
      }
    }
    return true;
  }

public:
  AiocbState* aiocb_state[4]; // accept, connect, send, recv
};

SocketNbioQueue::SocketNbioQueue()
  : fd_event_queue(true) {
}

void SocketNbioQueue::associate(SocketAiocb& aiocb, RetryStatus retry_status) {
  switch (retry_status) {
  case RetryStatus::WANT_RECV: {
    bool associate_ret =
      fd_event_queue.associate(
        aiocb.socket(),
        FdEvent::TYPE_READ_READY
      );
    CHECK(associate_ret);
  }
  break;

  case RetryStatus::WANT_SEND: {
    bool associate_ret =
      fd_event_queue.associate(
        aiocb.socket(),
        FdEvent::TYPE_WRITE_READY
      );
    CHECK(associate_ret);
  }
  break;

  default:
    CHECK(false);
    break;
  }
}

uint8_t SocketNbioQueue::get_aiocb_priority(const SocketAiocb& aiocb) {
  switch (aiocb.type()) {
  case SocketAiocb::Type::ACCEPT:
    return 0;
  case SocketAiocb::Type::CONNECT:
    return 1;
  case SocketAiocb::Type::RECV:
    return 3;
  case SocketAiocb::Type::RECVFROM:
    return 3;
  case SocketAiocb::Type::SEND:
    return 2;
  case SocketAiocb::Type::SENDFILE:
    return 2;
  default:
    CHECK(false);
    return 0;
  }
}

template <class AiocbType>
void SocketNbioQueue::log_partial_send(AiocbType& aiocb, size_t partial_send_len) {
  DLOG(DEBUG) << "partial send (" << partial_send_len << ") on " << aiocb;
}

template <class AiocbType> void SocketNbioQueue::log_retry(AiocbType& aiocb) {
  DLOG(DEBUG) << "retrying " << aiocb;
}

template <class AiocbType>
void SocketNbioQueue::log_wouldblock(AiocbType& aiocb, RetryStatus retry_status) {
  const char* retry_status_str;
  switch (retry_status) {
  case RetryStatus::WANT_RECV:
    retry_status_str = "read";
    break;
  case RetryStatus::WANT_SEND:
    retry_status_str = "write";
    break;
  default:
    CHECK(false);
    retry_status_str = "";
    break;
  }

  DLOG(DEBUG) << aiocb << " would block on " << retry_status_str;
}

SocketNbioQueue::RetryStatus SocketNbioQueue::retry(SocketAiocb& aiocb, size_t& partial_send_len) {
  switch (aiocb.type()) {
  case SocketAiocb::Type::ACCEPT:
    return retry_accept(static_cast<AcceptAiocb&>(aiocb));
  case SocketAiocb::Type::CONNECT:
    return retry_connect(static_cast<ConnectAiocb&>(aiocb), partial_send_len);
  case SocketAiocb::Type::RECV:
    return retry_recv(static_cast<RecvAiocb&>(aiocb));
  case SocketAiocb::Type::RECVFROM:
    return retry_recvfrom(static_cast<RecvfromAiocb&>(aiocb));
  case SocketAiocb::Type::SEND:
    return retry_send(static_cast<SendAiocb&>(aiocb), partial_send_len);
  case SocketAiocb::Type::SENDFILE:
    return retry_sendfile(static_cast<SendfileAiocb&>(aiocb), partial_send_len);
  default:
    CHECK(false);
    return RetryStatus::ERROR;
  }
}

SocketNbioQueue::RetryStatus SocketNbioQueue::retry_accept(AcceptAiocb& accept_aiocb) {
  log_retry(accept_aiocb);

  if (accept_aiocb.socket().set_blocking_mode(false)) {
    shared_ptr<SocketAddress> peername(new SocketAddress);
    shared_ptr<StreamSocket> accepted_socket
    = accept_aiocb.socket().accept(*peername);

    if (accepted_socket != NULL) {
      accept_aiocb.set_accepted_socket(accepted_socket);
      accept_aiocb.set_peername(peername);

      if (accept_aiocb.recv_buffer()) {
        if (accepted_socket->set_blocking_mode(false)) {
          ssize_t recv_ret =
            accepted_socket->recv(*accept_aiocb.recv_buffer(), 0);
          if (recv_ret > 0) {
            accept_aiocb.set_return(recv_ret);
          } else if (recv_ret == 0) {
            accept_aiocb.set_error(0);
            accept_aiocb.set_return(-1);
          } else if (accepted_socket->want_recv()) {
            accept_aiocb.set_return(0);
          }
        }
      } else {
        accept_aiocb.set_return(0);
      }

      log_completion(accept_aiocb);
      return RetryStatus::COMPLETE;
    } else if (accept_aiocb.socket().want_accept()) {
      log_wouldblock(accept_aiocb, RetryStatus::WANT_RECV);
      return RetryStatus::WANT_RECV;
    }
  }

  accept_aiocb.set_error(Exception::get_last_error_code());
  log_error(accept_aiocb);
  return RetryStatus::ERROR;
}

SocketNbioQueue::RetryStatus
SocketNbioQueue::retry_connect(
  ConnectAiocb& connect_aiocb,
  size_t& partial_send_len
) {
  log_retry(connect_aiocb);

  if (connect_aiocb.socket().set_blocking_mode(false)) {
    if (connect_aiocb.socket().connect(connect_aiocb.peername())) {
      if (connect_aiocb.send_buffer()) {
        return retry_send(
                 connect_aiocb,
                 *connect_aiocb.send_buffer(),
                 partial_send_len
               );
      } else {
        connect_aiocb.set_return(0);
        log_completion(connect_aiocb);
        return RetryStatus::COMPLETE;
      }
    } else if (connect_aiocb.socket().want_connect()) {
      log_wouldblock(connect_aiocb, RetryStatus::WANT_SEND);
      return RetryStatus::WANT_SEND;
    }
  }

  connect_aiocb.set_error(Exception::get_last_error_code());
  log_error(connect_aiocb);
  return RetryStatus::ERROR;
}

SocketNbioQueue::RetryStatus SocketNbioQueue::retry_recv(RecvAiocb& recv_aiocb) {
  log_retry(recv_aiocb);

  if (recv_aiocb.socket().set_blocking_mode(false)) {
    ssize_t recv_ret
    = recv_aiocb.socket().recv(
        *recv_aiocb.buffer(),
        recv_aiocb.flags()
      );

    if (recv_ret >= 0) {
      recv_aiocb.set_return(recv_ret);
      log_completion(recv_aiocb);
      return RetryStatus::COMPLETE;
    } else if (recv_aiocb.socket().want_recv()) {
      log_wouldblock(recv_aiocb, RetryStatus::WANT_RECV);
      return RetryStatus::WANT_RECV;
    } else if (recv_aiocb.socket().want_send()) {
      log_wouldblock(recv_aiocb, RetryStatus::WANT_SEND);
      return RetryStatus::WANT_SEND;
    }
  }

  recv_aiocb.set_error(Exception::get_last_error_code());
  log_error(recv_aiocb);
  return RetryStatus::ERROR;
}

SocketNbioQueue::RetryStatus SocketNbioQueue::retry_recvfrom(RecvfromAiocb& recvfrom_aiocb) {
  log_retry(recvfrom_aiocb);

  if (recvfrom_aiocb.socket().set_blocking_mode(false)) {
    ssize_t recv_ret
    = recvfrom_aiocb.socket().recvfrom(
        *recvfrom_aiocb.buffer(),
        recvfrom_aiocb.flags(),
        recvfrom_aiocb.peername()
      );

    if (recv_ret >= 0) {
      recvfrom_aiocb.set_return(recv_ret);
      log_completion(recvfrom_aiocb);
      return RetryStatus::COMPLETE;
    } else if (recvfrom_aiocb.socket().want_recv()) {
      log_wouldblock(recvfrom_aiocb, RetryStatus::WANT_RECV);
      return RetryStatus::WANT_RECV;
    } else if (recvfrom_aiocb.socket().want_send()) {
      log_wouldblock(recvfrom_aiocb, RetryStatus::WANT_SEND);
      return RetryStatus::WANT_SEND;
    }
  }

  recvfrom_aiocb.set_error(Exception::get_last_error_code());
  log_error(recvfrom_aiocb);
  return RetryStatus::ERROR;
}


SocketNbioQueue::RetryStatus
SocketNbioQueue::retry_send(
  SendAiocb& send_aiocb,
  size_t& partial_send_len
) {
  log_retry(send_aiocb);

  if (send_aiocb.socket().set_blocking_mode(false)) {
    return retry_send(send_aiocb, send_aiocb.buffer(), partial_send_len);
  } else {
    send_aiocb.set_error(Exception::get_last_error_code());
    log_error(send_aiocb);
    return RetryStatus::ERROR;
  }
}

template <class AiocbType>
SocketNbioQueue::RetryStatus
SocketNbioQueue::retry_send(
  AiocbType& aiocb,
  const Buffer& buffer,
  size_t& partial_send_len
) {
  ssize_t complete_send_ret, send_ret;
  if (buffer.get_next_buffer() == NULL) {
    complete_send_ret = buffer.size() - partial_send_len;
    send_ret
    = aiocb.socket().send(
        static_cast<const char*>(buffer) + partial_send_len,
        buffer.size() - partial_send_len,
        0
      );
  } else {
    vector<iovec> iov;
    complete_send_ret
    = Buffers::as_write_iovecs(buffer, partial_send_len, iov);
    send_ret = aiocb.socket().sendmsg(&iov[0], iov.size(), 0);
  }

  if (send_ret >= 0) {
    partial_send_len += static_cast<size_t>(send_ret);

    if (send_ret == complete_send_ret) {
      aiocb.set_return(partial_send_len);
      log_completion(aiocb);
      return RetryStatus::COMPLETE;
    } else {
      log_partial_send(aiocb, partial_send_len);
      return RetryStatus::WANT_SEND;
    }
  } else if (aiocb.socket().want_send()) {
    log_wouldblock(aiocb, RetryStatus::WANT_SEND);
    return RetryStatus::WANT_SEND;
  } else if (aiocb.socket().want_recv()) {
    log_wouldblock(aiocb, RetryStatus::WANT_RECV);
    return RetryStatus::WANT_RECV;
  } else {
    aiocb.set_error(Exception::get_last_error_code());
    log_error(aiocb);
    return RetryStatus::ERROR;
  }
}

SocketNbioQueue::RetryStatus
SocketNbioQueue::retry_sendfile(
  SendfileAiocb& sendfile_aiocb,
  size_t& partial_send_len
) {
  log_retry(sendfile_aiocb);

  if (sendfile_aiocb.socket().set_blocking_mode(false)) {
    ssize_t sendfile_ret
    = sendfile_aiocb.socket().sendfile(
        sendfile_aiocb.fd(),
        sendfile_aiocb.offset() + partial_send_len,
        sendfile_aiocb.nbytes() - partial_send_len
      );

    if (sendfile_ret >= 0) {
      partial_send_len += static_cast<size_t>(sendfile_ret);

      if (partial_send_len == sendfile_aiocb.nbytes()) {
        sendfile_aiocb.set_return(partial_send_len);
        log_completion(sendfile_aiocb);
        return RetryStatus::COMPLETE;
      } else {
        log_partial_send(sendfile_aiocb, partial_send_len);
        return RetryStatus::WANT_SEND;
      }
    } else if (sendfile_aiocb.socket().want_send()) {
      log_wouldblock(sendfile_aiocb, RetryStatus::WANT_SEND);
      return RetryStatus::WANT_SEND;
    } else if (sendfile_aiocb.socket().want_recv()) {
      log_wouldblock(sendfile_aiocb, RetryStatus::WANT_RECV);
      return RetryStatus::WANT_RECV;
    }
  }

  sendfile_aiocb.set_error(Exception::get_last_error_code());
  log_error(sendfile_aiocb);
  return RetryStatus::ERROR;
}

unique_ptr<SocketAiocb> SocketNbioQueue::timeddequeue(const Time& timeout) {
  Time timeout_remaining = timeout;

  Time start_time = Time::now();
  unique_ptr<SocketAiocb> ret(timeddequeue_fd_event_queue(Time::ZERO));
  if (ret) {
    return ret;
  }
  Time elapsed_time = Time::now() - start_time;
  if (timeout_remaining <= elapsed_time) {
    return unique_ptr<SocketAiocb>();
  }
  timeout_remaining -= elapsed_time;

  for (;;) {
    start_time = Time::now();
    ret = trydequeue_aiocb_queue();
    if (ret) {
      return ret;
    }
    elapsed_time = Time::now() - start_time;
    if (timeout_remaining <= elapsed_time) {
      return unique_ptr<SocketAiocb>();
    }
    timeout_remaining -= elapsed_time;

    start_time = Time::now();
    ret = timeddequeue_fd_event_queue(timeout_remaining);
    if (ret) {
      return ret;
    }
    elapsed_time = Time::now() - start_time;
    if (timeout_remaining <= elapsed_time) {
      return unique_ptr<SocketAiocb>();
    }
    timeout_remaining -= elapsed_time;
  }
}

unique_ptr<SocketAiocb> SocketNbioQueue::timeddequeue_fd_event_queue(const Time& timeout) {
  shared_ptr<FdEvent> fd_event = fd_event_queue.timeddequeue(timeout);
  if (!fd_event) {
    return unique_ptr<SocketAiocb>();
  }

  fd_t fd = fd_event->fd();

  auto socket_state_i = this->socket_state.find(fd);
  CHECK_NE(socket_state_i, this->socket_state.end());
  SocketState* socket_state = socket_state_i->second;

  uint16_t want_fd_event_types = 0;

  for (uint8_t aiocb_priority = 0; aiocb_priority < 4; ++aiocb_priority) {
    AiocbState* aiocb_state = socket_state->aiocb_state[aiocb_priority];
    if (aiocb_state != NULL) {
      SocketAiocb& aiocb = *aiocb_state->aiocb_;
      size_t& partial_send_len = aiocb_state->partial_send_len_;
      RetryStatus retry_status = retry(aiocb, partial_send_len);

      if (
        retry_status == RetryStatus::COMPLETE
        ||
        retry_status == RetryStatus::ERROR
      ) {
        unique_ptr<SocketAiocb> ret = move(aiocb_state->aiocb_);

        if (aiocb_state->next_aiocb_state_ == NULL) {
          socket_state->aiocb_state[aiocb_priority] = NULL;

          if (socket_state->empty()) {
            delete socket_state;
            this->socket_state.erase(socket_state_i);
            fd_event_queue.dissociate(fd);
          }
        } else {
          socket_state->aiocb_state[aiocb_priority]
          = aiocb_state->next_aiocb_state_;
          aiocb_state->next_aiocb_state_ = NULL;
          delete aiocb_state;
        }

        return ret;
      } else if (retry_status == RetryStatus::WANT_RECV) {
        want_fd_event_types |= FdEvent::TYPE_READ_READY;
        break;
      } else if (retry_status == RetryStatus::WANT_SEND) {
        want_fd_event_types |= FdEvent::TYPE_WRITE_READY;
        break;
      }
    }
  }

  CHECK_NE(want_fd_event_types, 0);
  bool associate_ret
  = fd_event_queue.associate(fd, want_fd_event_types);
  CHECK(associate_ret);

  return unique_ptr<SocketAiocb>();
}

unique_ptr<SocketAiocb> SocketNbioQueue::trydequeue_aiocb_queue() {
  unique_ptr<SocketAiocb> aiocb(aiocb_queue.trydequeue());
  if (!aiocb) {
    return unique_ptr<SocketAiocb>();
  }

  auto socket_state_i = this->socket_state.find(aiocb->socket());
  if (socket_state_i == this->socket_state.end()) {
    size_t partial_send_len = 0;
    RetryStatus retry_status = retry(*aiocb, partial_send_len);
    switch (retry_status) {
    case RetryStatus::COMPLETE:
    case RetryStatus::ERROR:
      return aiocb;
    default: {
      uint8_t aiocb_priority = get_aiocb_priority(*aiocb);
      SocketState* socket_state = new SocketState();
      this->socket_state[aiocb->socket()] = socket_state;
      associate(*aiocb, retry_status);
      socket_state->aiocb_state[aiocb_priority] = new AiocbState(move(aiocb), partial_send_len);
      // aiocb is empty here
    }
    break;
    }
  } else {
    SocketState* socket_state = socket_state_i->second;

    uint8_t aiocb_priority = get_aiocb_priority(*aiocb);

    // Check if there's already an SocketAiocb with an equal or higher priority
    // on this socket. If not, retry aiocb.
    bool should_retry_aiocb = true;
    for (
      int8_t check_aiocb_priority = aiocb_priority;
      check_aiocb_priority >= 0;
      --check_aiocb_priority
    ) {
      if (socket_state->aiocb_state[check_aiocb_priority] != NULL) {
        should_retry_aiocb = false;
        break;
      }
    }

    size_t partial_send_len = 0;
    if (should_retry_aiocb) {
      RetryStatus retry_status = retry(*aiocb, partial_send_len);
      switch (retry_status) {
      case RetryStatus::COMPLETE:
      case RetryStatus::ERROR:
        CHECK(!socket_state->empty());
        return aiocb;
      default:
        associate(*aiocb, retry_status);
        break;
      }
    }

    AiocbState* aiocb_state = new AiocbState(move(aiocb), partial_send_len);
    if (socket_state->aiocb_state[aiocb_priority] == NULL) {
      socket_state->aiocb_state[aiocb_priority] = aiocb_state;
    } else {
      AiocbState* last_aiocb_state
      = socket_state->aiocb_state[aiocb_priority];
      while (last_aiocb_state->next_aiocb_state_ != NULL) {
        last_aiocb_state = last_aiocb_state->next_aiocb_state_;
      }
      last_aiocb_state->next_aiocb_state_ = aiocb_state;
    }
  }

  return unique_ptr<SocketAiocb>();
}

unique_ptr<SocketAiocb> SocketNbioQueue::tryenqueue(unique_ptr<SocketAiocb> aiocb) {
  unique_ptr<SocketAiocb> ret = aiocb_queue.tryenqueue(move(aiocb));
  if (ret == NULL) {
    fd_event_queue.wake();
  }
  return ret;
}

void SocketNbioQueue::wake() {
  fd_event_queue.wake();
}
}
}
}
