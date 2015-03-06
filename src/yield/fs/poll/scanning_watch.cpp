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

#include "scanning_watch.hpp"
#include "yield/logging.hpp"

namespace yield {
namespace fs {
namespace poll {
bool ScanningWatch::equals(const Stat& left, const Stat& right) {
  return type(left) == type(right)
         &&
         left.mtime() == right.mtime()
         &&
         left.size() == right.size();
}

Directory::Entry::Type ScanningWatch::type(const Stat& stbuf) {
#ifdef _WIN32
  if (stbuf.ISDEV()) {
    return Directory::Entry::Type::DEV;
  } else if (stbuf.ISDIR()) {
    return Directory::Entry::Type::DIR;
  } else if (stbuf.ISREG()) {
    return Directory::Entry::Type::REG;
  }
#else
  if (stbuf.ISBLK()) {
    return Directory::Entry::Type::BLK;
  } else if (stbuf.ISCHR()) {
    return Directory::Entry::Type::CHR;
  } else if (stbuf.ISDIR()) {
    return Directory::Entry::Type::DIR;
  } else if (stbuf.ISFIFO()) {
    return Directory::Entry::Type::FIFO;
  } else if (stbuf.ISLNK()) {
    return Directory::Entry::Type::LNK;
  } else if (stbuf.ISREG()) {
    return Directory::Entry::Type::REG;
  } else if (stbuf.ISSOCK()) {
    return Directory::Entry::Type::SOCK;
  }
#endif
  else {
    CHECK(false);
    return Directory::Entry::Type::REG;
  }
}
}
}
}
