#include <string>
#include "yield/sockets/aio/socket_aiocb.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace yield {
namespace sockets {
namespace aio {
void SocketAiocb::init(off_t offset) {
  error_ = 0;
  offset_ = offset;
  return__ = 0;
#ifdef _WIN32
  static_assert(sizeof(overlapped_) == sizeof(::OVERLAPPED), "");
  memset(&overlapped_, 0, sizeof(overlapped_));
  overlapped_.Offset = static_cast<uint32_t>(offset);
  overlapped_.OffsetHigh = static_cast<uint32_t>(offset >> 32);
  this_ = this;
#else
  offset_ = offset;
#endif
}
}
}
}
