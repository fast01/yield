// yield/sockets/aio/win32/aio_queue.cpp

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

#include "../../win32/winsock.hpp"
#include "yield/buffer.hpp"
#include "yield/logging.hpp"
#include "yield/sockets/stream_socket.hpp"
#include "yield/sockets/aio/accept_aiocb.hpp"
#include "yield/sockets/aio/connect_aiocb.hpp"
#include "yield/sockets/aio/recv_aiocb.hpp"
#include "yield/sockets/aio/recvfrom_aiocb.hpp"
#include "yield/sockets/aio/send_aiocb.hpp"
#include "yield/sockets/aio/sendfile_aiocb.hpp"
#include "yield/sockets/aio/aio_queue.hpp"

namespace yield {
namespace sockets {
namespace aio {
using ::std::vector;

static LPFN_ACCEPTEX lpfnAcceptEx = NULL;
static LPFN_CONNECTEX lpfnConnectEx = NULL;
static LPFN_GETACCEPTEXSOCKADDRS lpfnGetAcceptExSockaddrs = NULL;
static LPFN_TRANSMITFILE lpfnTransmitFile = NULL;

AioQueue::AioQueue() {
  hIoCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (hIoCompletionPort == INVALID_HANDLE_VALUE) {
    throw Exception();
  }
}

bool AioQueue::associate(socket_t socket_) {
  return CreateIoCompletionPort(
           reinterpret_cast<fd_t>(socket_),
           hIoCompletionPort,
           0,
           0
         ) != INVALID_HANDLE_VALUE;
}

bool AioQueue::enqueue(YO_NEW_REF Event& event) {
  switch (event.get_type_id()) {
  case AcceptAiocb::TYPE_ID: {
    AcceptAiocb& accept_aiocb = static_cast<AcceptAiocb&>(event);

    log_enqueue(accept_aiocb);

    if (lpfnAcceptEx == NULL) {
      GUID GuidAcceptEx = WSAID_ACCEPTEX;
      DWORD dwBytes;
      WSAIoctl(
        accept_aiocb.get_socket(),
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &GuidAcceptEx,
        sizeof(GuidAcceptEx),
        &lpfnAcceptEx,
        sizeof(lpfnAcceptEx),
        &dwBytes,
        NULL,
        NULL
      );

      if (lpfnAcceptEx == NULL) {
        accept_aiocb.set_error(WSAGetLastError());
        log_error(accept_aiocb);
        return false;
      }
    }

    if (lpfnGetAcceptExSockaddrs == NULL) {
      DWORD dwBytes;
      GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
      WSAIoctl(
        accept_aiocb.get_socket(),
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &GuidGetAcceptExSockAddrs,
        sizeof(GuidGetAcceptExSockAddrs),
        &lpfnGetAcceptExSockaddrs,
        sizeof(lpfnGetAcceptExSockaddrs),
        &dwBytes,
        NULL,
        NULL
      );

      if (lpfnGetAcceptExSockaddrs == NULL) {
        accept_aiocb.set_error(WSAGetLastError());
        log_error(accept_aiocb);
        return false;
      }
    }

    StreamSocket* accepted_socket = accept_aiocb.get_socket().dup();

    if (accepted_socket != NULL) {
      accept_aiocb.set_accepted_socket(*accepted_socket);

      DWORD dwBytesReceived;
      DWORD dwLocalAddressLength
      = SocketAddress::len(accepted_socket->get_domain()) + 16;
      DWORD dwReceiveDataLength;
      DWORD dwRemoteAddressLength = dwLocalAddressLength;

      Buffer* recv_buffer = accept_aiocb.get_recv_buffer();
      if (recv_buffer != NULL) {
        if (
          recv_buffer->get_next_buffer() == NULL
          &&
          recv_buffer->capacity() - recv_buffer->size()
          >=
          dwLocalAddressLength + dwRemoteAddressLength
        ) {
          dwReceiveDataLength =
            recv_buffer->capacity() - recv_buffer->size() -
            (dwLocalAddressLength + dwRemoteAddressLength);
        } else {
          accept_aiocb.set_error(WSAEINVAL);
          log_error(accept_aiocb);
          return false;
        }
      } else {
        recv_buffer = new Buffer(dwLocalAddressLength + dwRemoteAddressLength);
        accept_aiocb.set_recv_buffer(recv_buffer);
        dwReceiveDataLength = 0;
      }

      if (
        lpfnAcceptEx(
          accept_aiocb.get_socket(),
          *accepted_socket,
          static_cast<char*>(*recv_buffer) + recv_buffer->size(),
          dwReceiveDataLength,
          dwLocalAddressLength,
          dwRemoteAddressLength,
          &dwBytesReceived,
          accept_aiocb
        ) == TRUE
        ||
        WSAGetLastError() == WSA_IO_PENDING
      ) {
        return true;
      }
    }

    accept_aiocb.set_error(WSAGetLastError());
    log_error(accept_aiocb);

    return false;
  }
  break;

  case ConnectAiocb::TYPE_ID: {
    ConnectAiocb& connect_aiocb = static_cast<ConnectAiocb&>(event);

    log_enqueue(connect_aiocb);

    if (lpfnConnectEx == NULL) {
      DWORD dwBytes;
      GUID GuidConnectEx = WSAID_CONNECTEX;
      WSAIoctl(
        connect_aiocb.get_socket(),
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &GuidConnectEx,
        sizeof(GuidConnectEx),
        &lpfnConnectEx,
        sizeof(lpfnConnectEx),
        &dwBytes,
        NULL,
        NULL
      );

      if (lpfnConnectEx == NULL) {
        connect_aiocb.set_error(WSAGetLastError());
        log_error(connect_aiocb);
        return false;
      }
    }

    const SocketAddress* peername
    = connect_aiocb.get_peername().filter(
        connect_aiocb.get_socket().get_domain()
      );

    if (peername != NULL) {
      PVOID lpSendBuffer;
      DWORD dwSendDataLength;
      if (connect_aiocb.get_send_buffer() != NULL) {
        lpSendBuffer = *connect_aiocb.get_send_buffer();
        dwSendDataLength = connect_aiocb.get_send_buffer()->size();
      } else {
        lpSendBuffer = NULL;
        dwSendDataLength = 0;
      }

      DWORD dwBytesSent;

      if (
        lpfnConnectEx(
          connect_aiocb.get_socket(),
          *peername,
          peername->len(),
          lpSendBuffer,
          dwSendDataLength,
          &dwBytesSent,
          connect_aiocb
        )
        ||
        WSAGetLastError() == WSA_IO_PENDING
      ) {
        return true;
      }
    }

    connect_aiocb.set_error(WSAGetLastError());
    log_error(connect_aiocb);

    return false;
  }
  break;

  case RecvAiocb::TYPE_ID: {
    RecvAiocb& recv_aiocb = static_cast<RecvAiocb&>(event);

    log_enqueue(recv_aiocb);

    DWORD dwFlags = static_cast<DWORD>(recv_aiocb.get_flags());

    if (recv_aiocb.get_buffer().get_next_buffer() == NULL) {
      iovec wsabuf = recv_aiocb.get_buffer().as_read_iovec();

      if (
        WSARecv(
          recv_aiocb.get_socket(),
          reinterpret_cast<WSABUF*>(&wsabuf),
          1,
          NULL,
          &dwFlags,
          recv_aiocb,
          NULL
        ) == 0
        ||
        WSAGetLastError() == WSA_IO_PENDING
      ) {
        return true;
      }
    } else { // Scatter I/O
      vector<iovec> wsabufs;
      Buffers::as_read_iovecs(recv_aiocb.get_buffer(), wsabufs);

      if (
        WSARecv(
          recv_aiocb.get_socket(),
          reinterpret_cast<WSABUF*>(&wsabufs[0]),
          wsabufs.size(),
          NULL,
          &dwFlags,
          recv_aiocb,
          NULL
        ) == 0
        ||
        WSAGetLastError() == WSA_IO_PENDING
      ) {
        return true;
      }
    }

    recv_aiocb.set_error(WSAGetLastError());
    log_error(recv_aiocb);

    return true;
  }
  break;

  case RecvfromAiocb::TYPE_ID: {
    RecvfromAiocb& recvfrom_aiocb = static_cast<RecvfromAiocb&>(event);

    log_enqueue(recvfrom_aiocb);

    DWORD dwFlags = static_cast<DWORD>(recvfrom_aiocb.get_flags());
    sockaddr* peername = recvfrom_aiocb.get_peername();
    socklen_t& peername_len = recvfrom_aiocb.get_peername_len();
    peername_len = SocketAddress::len(recvfrom_aiocb.get_socket().get_domain());

    if (recvfrom_aiocb.get_buffer().get_next_buffer() == NULL) {
      iovec wsabuf = recvfrom_aiocb.get_buffer().as_read_iovec();

      if (
        WSARecvFrom(
          recvfrom_aiocb.get_socket(),
          reinterpret_cast<WSABUF*>(&wsabuf),
          1,
          NULL,
          &dwFlags,
          peername,
          &peername_len,
          recvfrom_aiocb,
          NULL
        ) == 0
        ||
        WSAGetLastError() == WSA_IO_PENDING
      ) {
        return true;
      }
    } else { // Scatter I/O
      vector<iovec> wsabufs;
      Buffers::as_read_iovecs(recvfrom_aiocb.get_buffer(), wsabufs);

      if (
        WSARecvFrom(
          recvfrom_aiocb.get_socket(),
          reinterpret_cast<WSABUF*>(&wsabufs[0]),
          wsabufs.size(),
          NULL,
          &dwFlags,
          peername,
          &peername_len,
          recvfrom_aiocb,
          NULL
        ) == 0
        ||
        WSAGetLastError() == WSA_IO_PENDING
      ) {
        return true;
      }
    }

    recvfrom_aiocb.set_error(WSAGetLastError());
    log_error(recvfrom_aiocb);

    return true;
  }
  break;

  case SendAiocb::TYPE_ID: {
    SendAiocb& send_aiocb = static_cast<SendAiocb&>(event);

    log_enqueue(send_aiocb);

    if (send_aiocb.get_buffer().get_next_buffer() == NULL) {
      iovec wsabuf = send_aiocb.get_buffer().as_write_iovec();

      if (
        WSASend(
          send_aiocb.get_socket(),
          reinterpret_cast<WSABUF*>(&wsabuf),
          1,
          NULL,
          send_aiocb.get_flags(),
          send_aiocb,
          NULL
        ) == 0
        ||
        WSAGetLastError() == WSA_IO_PENDING
      ) {
        return true;
      }
    } else { // Gather I/O
      vector<iovec> wsabufs;
      Buffers::as_write_iovecs(send_aiocb.get_buffer(), wsabufs);

      if (
        WSASend(
          send_aiocb.get_socket(),
          reinterpret_cast<WSABUF*>(&wsabufs[0]),
          wsabufs.size(),
          NULL,
          send_aiocb.get_flags(),
          send_aiocb,
          NULL
        ) == 0
        ||
        WSAGetLastError() == WSA_IO_PENDING
      ) {
        return true;
      }
    }

    send_aiocb.set_error(WSAGetLastError());
    log_error(send_aiocb);

    return false;
  }
  break;

  case SendfileAiocb::TYPE_ID: {
    SendfileAiocb& sendfile_aiocb = static_cast<SendfileAiocb&>(event);

    if (lpfnTransmitFile == NULL) {
      GUID GuidTransmitFile = WSAID_TRANSMITFILE;
      DWORD dwBytes;
      WSAIoctl(
        sendfile_aiocb.get_socket(),
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &GuidTransmitFile,
        sizeof(GuidTransmitFile),
        &lpfnTransmitFile,
        sizeof(lpfnTransmitFile),
        &dwBytes,
        NULL,
        NULL
      );

      if (lpfnTransmitFile == NULL) {
        sendfile_aiocb.set_error(WSAGetLastError());
        log_error(sendfile_aiocb);
        return false;
      }
    }

    if (
      lpfnTransmitFile(
        sendfile_aiocb.get_socket(),
        sendfile_aiocb.get_fd(),
        sendfile_aiocb.get_nbytes(),
        0,
        sendfile_aiocb,
        NULL,
        0
      )
      ||
      WSAGetLastError() == WSA_IO_PENDING
    ) {
      return true;
    }

    sendfile_aiocb.set_error(WSAGetLastError());
    log_error(sendfile_aiocb);

    return false;
  }
  break;

  default: {
    return PostQueuedCompletionStatus(
             hIoCompletionPort,
             0,
             reinterpret_cast<ULONG_PTR>(&event),
             NULL
           )
           == TRUE;
  }
  break;
  }
}

template <class AiocbType> void AioQueue::log_completion(AiocbType& aiocb) {
  if (aiocb.get_return() >= 0) {
    DLOG(DEBUG) << "completed " << aiocb;
  } else {
    log_error(aiocb);
  }
}

template <class AiocbType> void AioQueue::log_enqueue(AiocbType& aiocb) {
  DLOG(DEBUG) << "enqueuing " << aiocb;
}

template <class AiocbType> void AioQueue::log_error(AiocbType& aiocb) {
  LOG(ERROR) << "error on " << aiocb;
}

YO_NEW_REF Event* AioQueue::timeddequeue(const Time& timeout) {
  DWORD dwBytesTransferred = 0;
  ULONG_PTR ulCompletionKey = 0;
  LPOVERLAPPED lpOverlapped = NULL;

  BOOL bRet
  = GetQueuedCompletionStatus(
      hIoCompletionPort,
      &dwBytesTransferred,
      &ulCompletionKey,
      &lpOverlapped,
      static_cast<DWORD>(timeout.ms())
    );

  if (lpOverlapped != NULL) {
    Aiocb& aiocb = Aiocb::cast(*lpOverlapped);

    if (bRet) {
      aiocb.set_return(dwBytesTransferred);
    } else {
      aiocb.set_error(GetLastError());
    }

    switch (aiocb.get_type_id()) {
    case AcceptAiocb::TYPE_ID: {
      AcceptAiocb& accept_aiocb = static_cast<AcceptAiocb&>(aiocb);

      if (accept_aiocb.get_error() == 0) {
        // accept_aiocb.return does NOT include the size of the
        // local and remote socket addresses.

        StreamSocket& accepted_socket
        = *accept_aiocb.get_accepted_socket();
        Buffer& recv_buffer = *accept_aiocb.get_recv_buffer();

        int optval = accept_aiocb.get_socket();
        setsockopt(
          accepted_socket,
          SOL_SOCKET,
          SO_UPDATE_ACCEPT_CONTEXT,
          reinterpret_cast<char*>(&optval),
          sizeof(optval)
        );

        DWORD dwLocalAddressLength
        = SocketAddress::len(accepted_socket.get_domain()) + 16;
        DWORD dwRemoteAddressLength = dwLocalAddressLength;
        DWORD dwReceiveDataLength =
          recv_buffer.capacity() - recv_buffer.size() -
          (dwLocalAddressLength + dwRemoteAddressLength);

        sockaddr* peername = NULL;
        socklen_t peernamelen;
        sockaddr* sockname;
        socklen_t socknamelen;

        lpfnGetAcceptExSockaddrs(
          static_cast<char*>(recv_buffer) + recv_buffer.size(),
          dwReceiveDataLength,
          dwLocalAddressLength,
          dwRemoteAddressLength,
          &sockname,
          &socknamelen,
          &peername,
          &peernamelen
        );

        if (peername != NULL) {
          accept_aiocb.set_peername(
            new SocketAddress(*peername, accepted_socket.get_domain())
          );

          if (accept_aiocb.get_return() > 0)
            recv_buffer.put(NULL, accept_aiocb.get_return());
        } else {
          accept_aiocb.set_error(WSAGetLastError());
        }
      }

      log_completion(accept_aiocb);
    }
    break;

    case ConnectAiocb::TYPE_ID: {
      log_completion(static_cast<ConnectAiocb&>(aiocb));
    }
    break;

    case RecvAiocb::TYPE_ID: {
      RecvAiocb& recv_aiocb = static_cast<RecvAiocb&>(aiocb);

      if (recv_aiocb.get_return() > 0) {
        Buffers::put(
          recv_aiocb.get_buffer(),
          NULL,
          static_cast<size_t>(recv_aiocb.get_return())
        );
      }

      log_completion(recv_aiocb);
    }
    break;

    case RecvfromAiocb::TYPE_ID: {
      RecvfromAiocb& recvfrom_aiocb = static_cast<RecvfromAiocb&>(aiocb);

      if (recvfrom_aiocb.get_return() > 0) {
        Buffers::put(
          recvfrom_aiocb.get_buffer(),
          NULL,
          static_cast<size_t>(recvfrom_aiocb.get_return())
        );
      }

      log_completion(recvfrom_aiocb);
    }
    break;

    case SendAiocb::TYPE_ID: {
      log_completion(static_cast<SendAiocb&>(aiocb));
    }
    break;
    }

    return &aiocb;
  } else if (ulCompletionKey != 0) {
    return reinterpret_cast<Event*>(ulCompletionKey);
  } else {
    return NULL;
  }
}
}
}
}
