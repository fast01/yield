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

#ifndef _YIELD_FS_DIRECTORY_HPP_
#define _YIELD_FS_DIRECTORY_HPP_

#include <memory>

#include "yield/fd_t.hpp"
#include "yield/fs/path.hpp"

#ifdef _WIN32
#include "yield/unique_fd.hpp"
#include "yield/fs/stat.hpp"
#else
#include <dirent.h>
#endif

namespace yield {
namespace fs {
/**
  A simple directory iterator, returned by FileSystem::opendir.
  Wraps a platform-specific directory handle.
*/
class Directory {
public:
  /**
    Metadata for a directory entry, mainly name and entry type.
  */
#ifdef _WIN32
  class Entry : public Stat {
#else
  class Entry {
#endif
  public:
    /**
      Type of a directory entry: directory, regular file, et al.
      Mirrors Stat::IS*.
    */
    enum class Type {
#ifndef _WIN32
      BLK,
      CHR,
#endif
#ifdef _WIN32
      DEV,
#endif
      DIR,
#ifndef _WIN32
      FIFO,
      LNK,
#endif
      REG,
#ifndef _WIN32
      SOCK
#endif
    };

  public:
    virtual ~Entry() {
    }

  public:
#ifndef _WIN32
    /**
      Check if this directory entry refers to a block device.
      @return true if this directory entry refers to a block device
    */
    bool ISBLK() const {
      return type() == Type::BLK;
    }

    /**
      Check if this directory entry refers to a character device.
      @return true if this directory entry refers to a character device
    */
    bool ISCHR() const {
      return type() == Type::CHR;
    }

    /**
      Check if this directory entry refers to a directory.
      @return true if this directory entry refers to a directory
    */
    bool ISDIR() const {
      return type() == Type::DIR;
    }

    /**
      Check if this directory entry refers to a named pipe.
      @return true if this directory entry refers to a named pipe
    */
    bool ISFIFO() const {
      return type() == Type::FIFO;
    }

    /**
      Check if this directory entry refers to a symbolic link.
      @return true if this directory entry refers to a symbolic link
    */
    bool ISLNK() const {
      return type() == Type::LNK;
    }

    /**
      Check if this directory entry refers to a regular file.
      @return true if this directory entry refers to a regular file
    */
    bool ISREG() const {
      return type() == Type::REG;
    }

    /**
      Check if this directory entry refers to a Unix socket.
      @return true if this directory entry refers to a Unix socket
    */
    bool ISSOCK() const {
      return type() == Type::SOCK;
    }
#endif

  public:
    /**
      Check if this directory entry refers to a hidden file.
      On Win32 hidden files have the FILE_ATTRIBUTE_HIDDEN attribute.
      On POSIX hidden files have a name starting with '.'.
      @return true if this directory entry refers to a hidden file
    */
    bool is_hidden() const;

    /**
      Check if this directory entry refers to a special file, e.g.
        Path::CURRENT_DIRECTORY or Path::PARENT_DIRECTORY.
      @return true if this directory entry refers to a special file
    */
    bool is_special() const;

  public:
    /**
      Get the name of this directory entry.
      @return the name of this directory entry
    */
    const Path& name() const {
      return name_;
    }

    /**
      Get the type of this directory entry.
      @return the type of this directory entry
    */
    Type type() const;

  private:
    friend class Directory;

#ifdef _WIN32
    Entry(const WIN32_FIND_DATA& win32_find_data);
    Entry& operator=(const WIN32_FIND_DATA&);
#else
    Entry(const Path& name, Type type)
      : name_(name), type_(type)
    { }

    void set_name(const Path& name) {
      this->name_ = name;
    }

    void set_type(Type type) {
      this->type_ = type;
    }
#endif

  private:
    Path name_;
#ifndef _WIN32
    Type type_;
#endif
  };

public:
#ifdef _WIN32
  Directory(unique_fd hDirectory);
#else
  Directory(DIR* dirp, const Path& path);
#endif

  virtual ~Directory() {
    close();
  }

public:
  bool close();

public:
#ifdef _WIN32
  operator fd_t() const {
    return *hDirectory_;
  }
#else
  operator DIR* () {
    return dirp_;
  }
#endif

public:
  ::std::unique_ptr<Entry> read() {
    Entry* entry = NULL;
    if (read(entry)) {
      return ::std::unique_ptr<Entry>(entry);
    } else {
      return ::std::unique_ptr<Entry>();
    }
  }

  bool read(Entry& entry) {
    Entry* p_dentry = &entry;
    return read(p_dentry);
  }

public:
  void rewind();

private:
  bool read(Entry*&);

private:
#ifdef _WIN32
  unique_fd hDirectory_;
  fd_t hFindFile_;
#else
  DIR* dirp_;
  Path path_;
#endif
};
}
}

#endif
