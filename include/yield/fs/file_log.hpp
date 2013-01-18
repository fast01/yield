// yield/fs/file_log.hpp

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

#ifndef _YIELD_FS_FILE_LOG_HPP_
#define _YIELD_FS_FILE_LOG_HPP_

#include "yield/log.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/path.hpp"

#include <fcntl.h> // For O_*

namespace yield {
namespace fs {
/**
  Log implementation that appends to a file.
*/
class FileLog : public Log {
public:
  /**
    Construct a FileLog that will append to a file at the given path with
      messages at or below the given level.
    The file is created on demand on the first log write.
    @param file_path path to the log file
    @param level level of the new log
  */
  FileLog(const Path& file_path, const Level& level = Level::DEBUG);

  /**
    Destruct the FileLog, closing the associated file if necessary.
  */
  ~FileLog();

private:
  // yield::Log
  void write(const char* str, size_t str_len);

private:
  File* file;
  Path file_path;
};
}
}

#endif
