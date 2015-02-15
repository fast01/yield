#include <string>
#include "yield/sockets/aio/aiocb.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace yield {
namespace sockets {
namespace aio {
void Aiocb::init() {
#ifdef _WIN32
  static_assert(sizeof(overlapped_) == sizeof(::OVERLAPPED), "");
  memset(&overlapped_, 0, sizeof(overlapped_));
  overlapped_.Offset = static_cast<uint32_t>(offset_);
  overlapped_.OffsetHigh = static_cast<uint32_t>(offset_ >> 32);
  this_ = this;
#endif
}
}
}
}
