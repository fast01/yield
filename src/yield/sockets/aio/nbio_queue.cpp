// yield/sockets/aio/nbio_queue.cpp

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

#include "yield/debug.hpp"
#include "yield/buffer.hpp"
#include "yield/exception.hpp"
#include "yield/log.hpp"
#include "yield/time.hpp"
#include "yield/sockets/stream_socket.hpp"
#include "yield/sockets/aio/accept_aiocb.hpp"
#include "yield/sockets/aio/nbio_queue.hpp"
#include "yield/sockets/aio/connect_aiocb.hpp"
#include "yield/sockets/aio/recv_aiocb.hpp"
#include "yield/sockets/aio/recvfrom_aiocb.hpp"
#include "yield/sockets/aio/send_aiocb.hpp"
#include "yield/sockets/aio/sendfile_aiocb.hpp"

namespace yield {
namespace sockets {
namespace aio {
using std::map;
using yield::poll::FdEvent;

class NbioQueue::AiocbState {
public:
  AiocbState(YO_NEW_REF Aiocb& aiocb, ssize_t partial_send_len)
    : aiocb(&aiocb),
      partial_send_len(partial_send_len) {
    next_aiocb_state = NULL;
  }

  ~AiocbState() {
    Aiocb::dec_ref(aiocb);
    delete next_aiocb_state;
  }

public:
  Aiocb* aiocb;
  AiocbState* next_aiocb_state;
  size_t partial_send_len;
};

class NbioQueue::SocketState {
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

NbioQueue::NbioQueue(YO_NEW_REF Log* log)
  : fd_event_queue(true), log(log) {
}

NbioQueue::~NbioQueue() {
  Log::dec_ref(log);
}

void NbioQueue::associate(Aiocb& aiocb, RetryStatus retry_status) {
  switch (retry_status) {
  case RETRY_STATUS_WANT_RECV: {
    bool associate_ret =
      fd_event_queue.associate(
        aiocb.get_socket(),
        FdEvent::TYPE_READ_READY
      );
    debug_assert_true(associate_ret);
  }
  break;

  case RETRY_STATUS_WANT_SEND: {
    bool associate_ret =
      fd_event_queue.associate(
        aiocb.get_socket(),
        FdEvent::TYPE_WRITE_READY
      );
    debug_assert_true(associate_ret);
  }
  break;

  default:
    debug_break();
    break;
  }
}

bool NbioQueue::enqueue(Event& event) {
  return fd_event_queue.enqueue(event);
}

uint8_t NbioQueue::get_aiocb_priority(const Aiocb& aiocb) {
  switch (aiocb.get_type_id()) {
  case AcceptAiocb::TYPE_ID:
    return 0;
  case ConnectAiocb::TYPE_ID:
    return 1;
  case RecvAiocb::TYPE_ID:
    return 3;
  case RecvfromAiocb::TYPE_ID:
    return 3;
  case SendAiocb::TYPE_ID:
    return 2;
  case SendfileAiocb::TYPE_ID:
    return 2;
  default:
    debug_break();
    return 0;
  }
}

template <class AiocbType> void NbioQueue::log_completion(AiocbType& aiocb) {
  if (log != NULL) {
    log->get_stream(Log::Level::DEBUG) <<
                                       get_type_name() << ": completed " << aiocb;
  }
}

template <class AiocbType> void NbioQueue::log_error(AiocbType& aiocb) {
  if (log != NULL) {
    log->get_stream(Log::Level::DEBUG) <<
                                       get_type_name() << ": error on " << aiocb;
  }
}

template <class AiocbType>
void NbioQueue::log_partial_send(AiocbType& aiocb, size_t partial_send_len) {
  if (log != NULL) {
    log->get_stream(Log::Level::DEBUG) <<
                                       get_type_name() << ": partial send (" << partial_send_len << ") on " << aiocb;
  }
}

template <class AiocbType> void NbioQueue::log_retry(AiocbType& aiocb) {
  if (log != NULL) {
    log->get_stream(Log::Level::DEBUG) <<
                                       get_type_name() << ": retrying " << aiocb;
  }
}

template <class AiocbType>
void NbioQueue::log_wouldblock(AiocbType& aiocb, RetryStatus retry_status) {
  if (log != NULL) {
    const char* retry_status_str;
    switch (retry_status) {
    case RETRY_STATUS_WANT_RECV:
      retry_status_str = "read";
      break;
    case RETRY_STATUS_WANT_SEND:
      retry_status_str = "write";
      break;
    default:
      debug_break();
      retry_status_str = "";
      break;
    }

    log->get_stream(Log::Level::DEBUG) <<
                                       get_type_name() << ": " <<
                                       aiocb << " would block on " << retry_status_str;
  }
}

NbioQueue::RetryStatus NbioQueue::retry(Aiocb& aiocb, size_t& partial_send_len) {
  switch (aiocb.get_type_id()) {
  case AcceptAiocb::TYPE_ID:
    return retry_accept(static_cast<AcceptAiocb&>(aiocb));
  case ConnectAiocb::TYPE_ID:
    return retry_connect(static_cast<ConnectAiocb&>(aiocb), partial_send_len);
  case RecvAiocb::TYPE_ID:
    return retry_recv(static_cast<RecvAiocb&>(aiocb));
  case RecvfromAiocb::TYPE_ID:
    return retry_recvfrom(static_cast<RecvfromAiocb&>(aiocb));
  case SendAiocb::TYPE_ID:
    return retry_send(static_cast<SendAiocb&>(aiocb), partial_send_len);
  case SendfileAiocb::TYPE_ID:
    return retry_sendfile(static_cast<SendfileAiocb&>(aiocb), partial_send_len);
  default:
    debug_break();
    return RETRY_STATUS_ERROR;
  }
}

NbioQueue::RetryStatus NbioQueue::retry_accept(AcceptAiocb& accept_aiocb) {
  log_retry(accept_aiocb);

  if (accept_aiocb.get_socket().set_blocking_mode(false)) {
    auto_Object<SocketAddress> peername = new SocketAddress;
    StreamSocket* accepted_socket
    = accept_aiocb.get_socket().accept(*peername);

    if (accepted_socket != NULL) {
      accept_aiocb.set_accepted_socket(*accepted_socket);
      accept_aiocb.set_peername(&peername->inc_ref());

      if (accept_aiocb.get_recv_buffer() != NULL) {
        if (accepted_socket->set_blocking_mode(false)) {
          ssize_t recv_ret =
            accepted_socket->recv(*accept_aiocb.get_recv_buffer(), 0);
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
      return RETRY_STATUS_COMPLETE;
    } else if (accept_aiocb.get_socket().want_accept()) {
      log_wouldblock(accept_aiocb, RETRY_STATUS_WANT_RECV);
      return RETRY_STATUS_WANT_RECV;
    }
  }

  accept_aiocb.set_error(Exception::get_last_error_code());
  log_error(accept_aiocb);
  return RETRY_STATUS_ERROR;
}

NbioQueue::RetryStatus
NbioQueue::retry_connect(
  ConnectAiocb& connect_aiocb,
  size_t& partial_send_len
) {
  log_retry(connect_aiocb);

  if (connect_aiocb.get_socket().set_blocking_mode(false)) {
    if (connect_aiocb.get_socket().connect(connect_aiocb.get_peername())) {
      if (connect_aiocb.get_send_buffer() != NULL) {
        return retry_send(
                 connect_aiocb,
                 *connect_aiocb.get_send_buffer(),
                 partial_send_len
               );
      } else {
        connect_aiocb.set_return(0);
        log_completion(connect_aiocb);
        return RETRY_STATUS_COMPLETE;
      }
    } else if (connect_aiocb.get_socket().want_connect()) {
      log_wouldblock(connect_aiocb, RETRY_STATUS_WANT_SEND);
      return RETRY_STATUS_WANT_SEND;
    }
  }

  connect_aiocb.set_error(Exception::get_last_error_code());
  log_error(connect_aiocb);
  return RETRY_STATUS_ERROR;
}

NbioQueue::RetryStatus NbioQueue::retry_recv(RecvAiocb& recv_aiocb) {
  log_retry(recv_aiocb);

  if (recv_aiocb.get_socket().set_blocking_mode(false)) {
    ssize_t recv_ret
    = recv_aiocb.get_socket().recv(
        recv_aiocb.get_buffer(),
        recv_aiocb.get_flags()
      );

    if (recv_ret >= 0) {
      recv_aiocb.set_return(recv_ret);
      log_completion(recv_aiocb);
      return RETRY_STATUS_COMPLETE;
    } else if (recv_aiocb.get_socket().want_recv()) {
      log_wouldblock(recv_aiocb, RETRY_STATUS_WANT_RECV);
      return RETRY_STATUS_WANT_RECV;
    } else if (recv_aiocb.get_socket().want_send()) {
      log_wouldblock(recv_aiocb, RETRY_STATUS_WANT_SEND);
      return RETRY_STATUS_WANT_SEND;
    }
  }

  recv_aiocb.set_error(Exception::get_last_error_code());
  log_error(recv_aiocb);
  return RETRY_STATUS_ERROR;
}

NbioQueue::RetryStatus NbioQueue::retry_recvfrom(RecvfromAiocb& recvfrom_aiocb) {
  log_retry(recvfrom_aiocb);

  if (recvfrom_aiocb.get_socket().set_blocking_mode(false)) {
    ssize_t recv_ret
    = recvfrom_aiocb.get_socket().recvfrom(
        recvfrom_aiocb.get_buffer(),
        recvfrom_aiocb.get_flags(),
        recvfrom_aiocb.get_peername()
      );

    if (recv_ret >= 0) {
      recvfrom_aiocb.set_return(recv_ret);
      log_completion(recvfrom_aiocb);
      return RETRY_STATUS_COMPLETE;
    } else if (recvfrom_aiocb.get_socket().want_recv()) {
      log_wouldblock(recvfrom_aiocb, RETRY_STATUS_WANT_RECV);
      return RETRY_STATUS_WANT_RECV;
    } else if (recvfrom_aiocb.get_socket().want_send()) {
      log_wouldblock(recvfrom_aiocb, RETRY_STATUS_WANT_SEND);
      return RETRY_STATUS_WANT_SEND;
    }
  }

  recvfrom_aiocb.set_error(Exception::get_last_error_code());
  log_error(recvfrom_aiocb);
  return RETRY_STATUS_ERROR;
}


NbioQueue::RetryStatus
NbioQueue::retry_send(
  SendAiocb& send_aiocb,
  size_t& partial_send_len
) {
  log_retry(send_aiocb);

  if (send_aiocb.get_socket().set_blocking_mode(false)) {
    return retry_send(send_aiocb, send_aiocb.get_buffer(), partial_send_len);
  } else {
    send_aiocb.set_error(Exception::get_last_error_code());
    log_error(send_aiocb);
    return RETRY_STATUS_ERROR;
  }
}

template <class AiocbType>
NbioQueue::RetryStatus
NbioQueue::retry_send(
  AiocbType& aiocb,
  const Buffer& buffer,
  size_t& partial_send_len
) {
  ssize_t complete_send_ret, send_ret;
  if (buffer.get_next_buffer() == NULL) {
    complete_send_ret = buffer.size() - partial_send_len;
    send_ret
    = aiocb.get_socket().send(
        static_cast<const char*>(buffer) + partial_send_len,
        buffer.size() - partial_send_len,
        0
      );
  } else {
    vector<iovec> iov;
    complete_send_ret
    = Buffers::as_write_iovecs(buffer, partial_send_len, iov);
    send_ret = aiocb.get_socket().sendmsg(&iov[0], iov.size(), 0);
  }

  if (send_ret >= 0) {
    partial_send_len += static_cast<size_t>(send_ret);

    if (send_ret == complete_send_ret) {
      aiocb.set_return(partial_send_len);
      log_completion(aiocb);
      return RETRY_STATUS_COMPLETE;
    } else {
      log_partial_send(aiocb, partial_send_len);
      return RETRY_STATUS_WANT_SEND;
    }
  } else if (aiocb.get_socket().want_send()) {
    log_wouldblock(aiocb, RETRY_STATUS_WANT_SEND);
    return RETRY_STATUS_WANT_SEND;
  } else if (aiocb.get_socket().want_recv()) {
    log_wouldblock(aiocb, RETRY_STATUS_WANT_RECV);
    return RETRY_STATUS_WANT_RECV;
  } else {
    aiocb.set_error(Exception::get_last_error_code());
    log_error(aiocb);
    return RETRY_STATUS_ERROR;
  }
}

NbioQueue::RetryStatus
NbioQueue::retry_sendfile(
  SendfileAiocb& sendfile_aiocb,
  size_t& partial_send_len
) {
  log_retry(sendfile_aiocb);

  if (sendfile_aiocb.get_socket().set_blocking_mode(false)) {
    ssize_t sendfile_ret
    = sendfile_aiocb.get_socket().sendfile(
        sendfile_aiocb.get_fd(),
        sendfile_aiocb.get_offset() + partial_send_len,
        sendfile_aiocb.get_nbytes() - partial_send_len
      );

    if (sendfile_ret >= 0) {
      partial_send_len += static_cast<size_t>(sendfile_ret);

      if (partial_send_len == sendfile_aiocb.get_nbytes()) {
        sendfile_aiocb.set_return(partial_send_len);
        log_completion(sendfile_aiocb);
        return RETRY_STATUS_COMPLETE;
      } else {
        log_partial_send(sendfile_aiocb, partial_send_len);
        return RETRY_STATUS_WANT_SEND;
      }
    } else if (sendfile_aiocb.get_socket().want_send()) {
      log_wouldblock(sendfile_aiocb, RETRY_STATUS_WANT_SEND);
      return RETRY_STATUS_WANT_SEND;
    } else if (sendfile_aiocb.get_socket().want_recv()) {
      log_wouldblock(sendfile_aiocb, RETRY_STATUS_WANT_RECV);
      return RETRY_STATUS_WANT_RECV;
    }
  }

  sendfile_aiocb.set_error(Exception::get_last_error_code());
  log_error(sendfile_aiocb);
  return RETRY_STATUS_ERROR;
}

Event* NbioQueue::timeddequeue(const Time& timeout) {
  Time timeout_remaining = timeout;

  for (;;) {
    Time start_time = Time::now();

    Event* event = fd_event_queue.timeddequeue(timeout_remaining);

    if (event != NULL) {
      switch (event->get_type_id()) {
      case FdEvent::TYPE_ID: {
        FdEvent* fd_event = static_cast<FdEvent*>(event);
        fd_t fd = fd_event->get_fd();
        FdEvent::dec_ref(*fd_event);

        map<fd_t, SocketState*>::iterator socket_state_i
        = this->socket_state.find(fd);
        debug_assert_ne(socket_state_i, this->socket_state.end());
        SocketState* socket_state = socket_state_i->second;

        uint16_t want_fd_event_types = 0;

        for (uint8_t aiocb_priority = 0; aiocb_priority < 4; ++aiocb_priority) {
          AiocbState* aiocb_state = socket_state->aiocb_state[aiocb_priority];
          if (aiocb_state != NULL) {
            Aiocb* aiocb = aiocb_state->aiocb;
            size_t& partial_send_len = aiocb_state->partial_send_len;
            RetryStatus retry_status = retry(*aiocb, partial_send_len);

            if (
              retry_status == RETRY_STATUS_COMPLETE
              ||
              retry_status == RETRY_STATUS_ERROR
            ) {
              aiocb_state->aiocb = NULL;

              if (aiocb_state->next_aiocb_state == NULL) {
                socket_state->aiocb_state[aiocb_priority] = NULL;

                if (socket_state->empty()) {
                  delete socket_state;
                  this->socket_state.erase(socket_state_i);
                  fd_event_queue.dissociate(fd);
                }
              } else {
                socket_state->aiocb_state[aiocb_priority]
                = aiocb_state->next_aiocb_state;
                aiocb_state->next_aiocb_state = NULL;
                delete aiocb_state;
              }

              return aiocb;
            } else if (retry_status == RETRY_STATUS_WANT_RECV) {
              want_fd_event_types |= FdEvent::TYPE_READ_READY;
              break;
            } else if (retry_status == RETRY_STATUS_WANT_SEND) {
              want_fd_event_types |= FdEvent::TYPE_WRITE_READY;
              break;
            }
          }
        }

        debug_assert_ne(want_fd_event_types, 0);
        bool associate_ret
        = fd_event_queue.associate(fd, want_fd_event_types);
        debug_assert_true(associate_ret);
      }
      break;

      case AcceptAiocb::TYPE_ID:
      case ConnectAiocb::TYPE_ID:
      case RecvAiocb::TYPE_ID:
      case RecvfromAiocb::TYPE_ID:
      case SendAiocb::TYPE_ID:
      case SendfileAiocb::TYPE_ID: {
        Aiocb* aiocb = static_cast<Aiocb*>(event);

        map<fd_t, SocketState*>::iterator socket_state_i
        = this->socket_state.find(aiocb->get_socket());

        if (socket_state_i == this->socket_state.end()) {
          size_t partial_send_len = 0;
          RetryStatus retry_status = retry(*aiocb, partial_send_len);
          switch (retry_status) {
          case RETRY_STATUS_COMPLETE:
          case RETRY_STATUS_ERROR:
            return aiocb;
          default: {
            AiocbState* aiocb_state = new AiocbState(*aiocb, partial_send_len);
            SocketState* socket_state = new SocketState();
            uint8_t aiocb_priority = get_aiocb_priority(*aiocb);
            socket_state->aiocb_state[aiocb_priority] = aiocb_state;
            this->socket_state[aiocb->get_socket()] = socket_state;
            associate(*aiocb, retry_status);
          }
          break;
          }
        } else {
          SocketState* socket_state = socket_state_i->second;

          uint8_t aiocb_priority = get_aiocb_priority(*aiocb);

          // Check if there's already an Aiocb with an equal or higher priority
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
            case RETRY_STATUS_COMPLETE:
            case RETRY_STATUS_ERROR:
              debug_assert_false(socket_state->empty());
              return aiocb;
            default:
              associate(*aiocb, retry_status);
              break;
            }
          }

          AiocbState* aiocb_state = new AiocbState(*aiocb, partial_send_len);
          if (socket_state->aiocb_state[aiocb_priority] == NULL) {
            socket_state->aiocb_state[aiocb_priority] = aiocb_state;
          } else {
            AiocbState* last_aiocb_state
            = socket_state->aiocb_state[aiocb_priority];
            while (last_aiocb_state->next_aiocb_state != NULL) {
              last_aiocb_state = last_aiocb_state->next_aiocb_state;
            }
            last_aiocb_state->next_aiocb_state = aiocb_state;
          }
        }
      }
      break;

      default:
        return event;
      }
    } else if (timeout_remaining > static_cast<uint64_t>(0)) {
      Time elapsed_time = Time::now() - start_time;
      if (timeout_remaining > elapsed_time) {
        timeout_remaining -= elapsed_time;
      } else {
        timeout_remaining = 0;
      }
    } else {
      return NULL;
    }
  }
}
}
}
}
