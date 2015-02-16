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

#include "./win32_socket_aio_queue.hpp"
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

namespace yield {
namespace sockets {
namespace aio {
namespace win32 {
using ::std::make_shared;
using ::std::shared_ptr;
using ::std::unique_ptr;
using ::std::vector;

static LPFN_ACCEPTEX lpfnAcceptEx = NULL;
static LPFN_CONNECTEX lpfnConnectEx = NULL;
static LPFN_GETACCEPTEXSOCKADDRS lpfnGetAcceptExSockaddrs = NULL;
static LPFN_TRANSMITFILE lpfnTransmitFile = NULL;

Win32SocketAioQueue::Win32SocketAioQueue() {
  hIoCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (hIoCompletionPort == INVALID_HANDLE_VALUE) {
    throw Exception();
  }
}

Win32SocketAioQueue::~Win32SocketAioQueue() {
  ::CloseHandle(hIoCompletionPort);
}

bool Win32SocketAioQueue::associate(socket_t socket_) {
  return CreateIoCompletionPort(
           reinterpret_cast<fd_t>(socket_),
           hIoCompletionPort,
           0,
           0
         ) != INVALID_HANDLE_VALUE;
}

unique_ptr<SocketAiocb> Win32SocketAioQueue::timeddequeue(const Time& timeout) {
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

  if (lpOverlapped == NULL) {
    return NULL;
  }

  unique_ptr<SocketAiocb> aiocb;
  {
    SocketAiocb* aiocb_temp;
    memcpy_s(
      &aiocb_temp,
      sizeof(aiocb_temp),
      reinterpret_cast<char*>(&lpOverlapped) + sizeof(::OVERLAPPED),
      sizeof(aiocb_temp)
    );
    aiocb.reset(aiocb_temp);
  }

  if (bRet) {
    aiocb->set_return(dwBytesTransferred);
  } else {
    aiocb->set_error(GetLastError());
  }

  switch (aiocb->type()) {
    case SocketAiocb::Type::ACCEPT: {
    AcceptAiocb& accept_aiocb = static_cast<AcceptAiocb&>(*aiocb);

    if (accept_aiocb.error() == 0) {
      // accept_aiocb.return does NOT include the size of the
      // local and remote socket addresses.

      StreamSocket& accepted_socket
      = *accept_aiocb.accepted_socket();
      Buffer& recv_buffer = *accept_aiocb.recv_buffer();

      int optval = accept_aiocb.socket();
      setsockopt(
        accepted_socket,
        SOL_SOCKET,
        SO_UPDATE_ACCEPT_CONTEXT,
        reinterpret_cast<char*>(&optval),
        sizeof(optval)
      );

      DWORD dwLocalAddressLength
      = SocketAddress::len(accepted_socket.domain()) + 16;
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
          make_shared<SocketAddress>(SocketAddress(*peername, accepted_socket.domain()))
        );

        if (accept_aiocb.return_() > 0)
          recv_buffer.put(NULL, accept_aiocb.return_());
      } else {
        accept_aiocb.set_error(WSAGetLastError());
      }
    }

    log_completion(accept_aiocb);
  }
  break;

  case SocketAiocb::Type::CONNECT: {
    log_completion(static_cast<ConnectAiocb&>(*aiocb));
  }
  break;

  case SocketAiocb::Type::RECV: {
    RecvAiocb& recv_aiocb = static_cast<RecvAiocb&>(*aiocb);

    if (recv_aiocb.return_() > 0) {
      Buffers::put(
        *recv_aiocb.buffer(),
        NULL,
        static_cast<size_t>(recv_aiocb.return_())
      );
    }

    log_completion(recv_aiocb);
  }
  break;

  case SocketAiocb::Type::RECVFROM: {
    RecvfromAiocb& recvfrom_aiocb = static_cast<RecvfromAiocb&>(*aiocb);

    if (recvfrom_aiocb.return_() > 0) {
      Buffers::put(
        *recvfrom_aiocb.buffer(),
        NULL,
        static_cast<size_t>(recvfrom_aiocb.return_())
      );
    }

    log_completion(recvfrom_aiocb);
  }
  break;

  case SocketAiocb::Type::SEND: {
    log_completion(static_cast<SendAiocb&>(*aiocb));
  }
  break;

  case SocketAiocb::Type::SENDFILE: {
    log_completion(static_cast<SendfileAiocb&>(*aiocb));
  }
  break;
  }

  return aiocb;
}

unique_ptr<SocketAiocb> Win32SocketAioQueue::tryenqueue(unique_ptr<SocketAiocb> unique_aiocb) {
  SocketAiocb* aiocb = unique_aiocb.release();
  ::OVERLAPPED* lpOverlapped = reinterpret_cast<::OVERLAPPED*>(&aiocb->overlapped_);

  switch (aiocb->type()) {
  case SocketAiocb::Type::ACCEPT: {
    AcceptAiocb& accept_aiocb = static_cast<AcceptAiocb&>(*aiocb);

    log_enqueue(accept_aiocb);

    if (lpfnAcceptEx == NULL) {
      GUID GuidAcceptEx = WSAID_ACCEPTEX;
      DWORD dwBytes;
      WSAIoctl(
        accept_aiocb.socket(),
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
        return unique_ptr<SocketAiocb>(aiocb);
      }
    }

    if (lpfnGetAcceptExSockaddrs == NULL) {
      DWORD dwBytes;
      GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
      WSAIoctl(
        accept_aiocb.socket(),
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
        return NULL;
      }
    }

    unique_ptr<StreamSocket> accepted_socket = accept_aiocb.socket().dup();

    if (accepted_socket != NULL) {
      accept_aiocb.set_accepted_socket(make_shared<StreamSocket>(accepted_socket));

      DWORD dwBytesReceived;
      DWORD dwLocalAddressLength
      = SocketAddress::len(accepted_socket->domain()) + 16;
      DWORD dwReceiveDataLength;
      DWORD dwRemoteAddressLength = dwLocalAddressLength;

      shared_ptr<Buffer> recv_buffer = accept_aiocb.recv_buffer();
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
        recv_buffer = make_shared<Buffer>(dwLocalAddressLength + dwRemoteAddressLength);
        accept_aiocb.set_recv_buffer(recv_buffer);
        dwReceiveDataLength = 0;
      }

      if (
        lpfnAcceptEx(
          accept_aiocb.socket(),
          *accepted_socket,
          static_cast<char*>(*recv_buffer) + recv_buffer->size(),
          dwReceiveDataLength,
          dwLocalAddressLength,
          dwRemoteAddressLength,
          &dwBytesReceived,
          lpOverlapped
        ) == TRUE
        ||
        WSAGetLastError() == WSA_IO_PENDING
      ) {
        return NULL;
      }
    }

    accept_aiocb.set_error(WSAGetLastError());
    log_error(accept_aiocb);

    return unique_ptr<SocketAiocb>(aiocb);
  }
  break;

  case SocketAiocb::Type::CONNECT: {
    ConnectAiocb& connect_aiocb = static_cast<ConnectAiocb&>(*aiocb);

    log_enqueue(connect_aiocb);

    if (lpfnConnectEx == NULL) {
      DWORD dwBytes;
      GUID GuidConnectEx = WSAID_CONNECTEX;
      WSAIoctl(
        connect_aiocb.socket(),
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
    = connect_aiocb.peername().filter(
        connect_aiocb.socket().domain()
      );

    if (peername != NULL) {
      PVOID lpSendBuffer;
      DWORD dwSendDataLength;
      if (connect_aiocb.send_buffer() != NULL) {
        lpSendBuffer = *connect_aiocb.send_buffer();
        dwSendDataLength = connect_aiocb.send_buffer()->size();
      } else {
        lpSendBuffer = NULL;
        dwSendDataLength = 0;
      }

      DWORD dwBytesSent;

      if (
        lpfnConnectEx(
          connect_aiocb.socket(),
          *peername,
          peername->len(),
          lpSendBuffer,
          dwSendDataLength,
          &dwBytesSent,
          lpOverlapped
        )
        ||
        WSAGetLastError() == WSA_IO_PENDING
      ) {
        return NULL;
      }
    }

    connect_aiocb.set_error(WSAGetLastError());
    log_error(connect_aiocb);
    return unique_ptr<SocketAiocb>(aiocb);
  }
  break;

  case SocketAiocb::Type::RECV: {
    RecvAiocb& recv_aiocb = static_cast<RecvAiocb&>(*aiocb);

    log_enqueue(recv_aiocb);

    DWORD dwFlags = static_cast<DWORD>(recv_aiocb.flags());

    if (recv_aiocb.buffer()->get_next_buffer() == NULL) {
      iovec wsabuf = recv_aiocb.buffer()->as_read_iovec();

      if (
        WSARecv(
          recv_aiocb.socket(),
          reinterpret_cast<WSABUF*>(&wsabuf),
          1,
          NULL,
          &dwFlags,
          lpOverlapped,
          NULL
        ) == 0
        ||
        WSAGetLastError() == WSA_IO_PENDING
      ) {
        return NULL;
      }
    } else { // Scatter I/O
      vector<iovec> wsabufs;
      Buffers::as_read_iovecs(*recv_aiocb.buffer(), wsabufs);

      if (
        WSARecv(
          recv_aiocb.socket(),
          reinterpret_cast<WSABUF*>(&wsabufs[0]),
          wsabufs.size(),
          NULL,
          &dwFlags,
          lpOverlapped,
          NULL
        ) == 0
        ||
        WSAGetLastError() == WSA_IO_PENDING
      ) {
        return NULL;
      }
    }

    recv_aiocb.set_error(WSAGetLastError());
    log_error(recv_aiocb);
    return unique_ptr<SocketAiocb>(aiocb);
  }
  break;

  case SocketAiocb::Type::RECVFROM: {
    RecvfromAiocb& recvfrom_aiocb = static_cast<RecvfromAiocb&>(*aiocb);

    log_enqueue(recvfrom_aiocb);

    DWORD dwFlags = static_cast<DWORD>(recvfrom_aiocb.flags());
    sockaddr* peername = recvfrom_aiocb.peername();
    socklen_t& peername_len = recvfrom_aiocb.peername_len();
    peername_len = SocketAddress::len(recvfrom_aiocb.socket().domain());

    if (recvfrom_aiocb.buffer()->get_next_buffer() == NULL) {
      iovec wsabuf = recvfrom_aiocb.buffer()->as_read_iovec();

      if (
        WSARecvFrom(
          recvfrom_aiocb.socket(),
          reinterpret_cast<WSABUF*>(&wsabuf),
          1,
          NULL,
          &dwFlags,
          peername,
          &peername_len,
          lpOverlapped,
          NULL
        ) == 0
        ||
        WSAGetLastError() == WSA_IO_PENDING
      ) {
        return NULL;
      }
    } else { // Scatter I/O
      vector<iovec> wsabufs;
      Buffers::as_read_iovecs(*recvfrom_aiocb.buffer(), wsabufs);

      if (
        WSARecvFrom(
          recvfrom_aiocb.socket(),
          reinterpret_cast<WSABUF*>(&wsabufs[0]),
          wsabufs.size(),
          NULL,
          &dwFlags,
          peername,
          &peername_len,
          lpOverlapped,
          NULL
        ) == 0
        ||
        WSAGetLastError() == WSA_IO_PENDING
      ) {
        return NULL;
      }
    }

    recvfrom_aiocb.set_error(WSAGetLastError());
    log_error(recvfrom_aiocb);
    return unique_ptr<SocketAiocb>(aiocb);
  }
  break;

  case SocketAiocb::Type::SEND: {
    SendAiocb& send_aiocb = static_cast<SendAiocb&>(*aiocb);

    log_enqueue(send_aiocb);

    if (send_aiocb.buffer().get_next_buffer() == NULL) {
      iovec wsabuf = send_aiocb.buffer().as_write_iovec();

      if (
        WSASend(
          send_aiocb.socket(),
          reinterpret_cast<WSABUF*>(&wsabuf),
          1,
          NULL,
          send_aiocb.flags(),
          lpOverlapped,
          NULL
        ) == 0
        ||
        WSAGetLastError() == WSA_IO_PENDING
      ) {
        return NULL;
      }
    } else { // Gather I/O
      vector<iovec> wsabufs;
      Buffers::as_write_iovecs(send_aiocb.buffer(), wsabufs);

      if (
        WSASend(
          send_aiocb.socket(),
          reinterpret_cast<WSABUF*>(&wsabufs[0]),
          wsabufs.size(),
          NULL,
          send_aiocb.flags(),
          lpOverlapped,
          NULL
        ) == 0
        ||
        WSAGetLastError() == WSA_IO_PENDING
      ) {
        return NULL;
      }
    }

    send_aiocb.set_error(WSAGetLastError());
    log_error(send_aiocb);
    return unique_ptr<SocketAiocb>(aiocb);
  }
  break;

  case SocketAiocb::Type::SENDFILE: {
    SendfileAiocb& sendfile_aiocb = static_cast<SendfileAiocb&>(*aiocb);

    if (lpfnTransmitFile == NULL) {
      GUID GuidTransmitFile = WSAID_TRANSMITFILE;
      DWORD dwBytes;
      WSAIoctl(
        sendfile_aiocb.socket(),
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
        sendfile_aiocb.socket(),
        sendfile_aiocb.fd(),
        sendfile_aiocb.nbytes(),
        0,
        lpOverlapped,
        NULL,
        0
      )
      ||
      WSAGetLastError() == WSA_IO_PENDING
    ) {
      return NULL;
    }

    sendfile_aiocb.set_error(WSAGetLastError());
    log_error(sendfile_aiocb);
    return unique_ptr<SocketAiocb>(aiocb);
  }
  break;
  }
}

void Win32SocketAioQueue::wake() {
  //return PostQueuedCompletionStatus(
  //          hIoCompletionPort,
  //          0,
  //          reinterpret_cast<ULONG_PTR>(&event),
  //          NULL
  //        )
  //        == TRUE;
  ::PostQueuedCompletionStatus(hIoCompletionPort, 0, 0, NULL);
}
}
}
}
}
