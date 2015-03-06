// file_system_test.cpp

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

#include "yield/exception.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/stat.hpp"
#include "gtest/gtest.h"

#include <fcntl.h>
#ifndef _WIN32
#include <sys/statvfs.h>
#endif

namespace yield {
namespace fs {
using ::std::unique_ptr;

class FileSystemTest : public ::testing::Test {
public:
  virtual ~FileSystemTest() { }

protected:
  FileSystemTest()
    : test_dir_name_("file_system_test"),
      test_file_name_("file_system_test.txt"),
      test_link_name_("file_system_test_link.txt")
  { }

  const Path& test_dir_name() const {
    return test_dir_name_;
  }

  const Path& test_file_name() const {
    return test_file_name_;
  }

  const Path& test_link_name() const {
    return test_link_name_;
  }

  // ::testing::Test
  void SetUp() {
    TearDown();
    FileSystem().touch(test_file_name());
  }

  void TearDown() {
    FileSystem().rmtree(test_dir_name());
    FileSystem().unlink(test_file_name());
    FileSystem().unlink(test_link_name());
  }

private:
  Path test_dir_name_, test_file_name_, test_link_name_;
};


#ifndef _WIN32
TEST_F(FileSystemTest, access) {
  ASSERT_TRUE(FileSystem().access(test_file_name(), F_OK));
}

TEST_F(FileSystemTest, chmod) {
  unique_ptr<Stat> stbuf = FileSystem().stat(test_file_name());
  ASSERT_TRUE(
    FileSystem().chmod(test_file_name(), stbuf->mode())
  );
}

TEST_F(FileSystemTest, chown) {
  unique_ptr<Stat> stbuf = FileSystem().stat(test_file_name());

  ASSERT_TRUE(
    FileSystem().chown(test_file_name(), stbuf->uid())
  );

  ASSERT_TRUE(
    FileSystem().chown(
      test_file_name(),
      stbuf->uid(),
      stbuf->gid()
    )
  );
}
#endif

TEST_F(FileSystemTest, creat) {
  unique_ptr<File> file = FileSystem().creat(test_file_name());
  if (file == NULL) {
    throw Exception();
  }
}

TEST_F(FileSystemTest, exists) {
  if (!FileSystem().exists(test_file_name())) {
    throw Exception();
  }

  ASSERT_FALSE(FileSystem().exists(Path("some other file.txt")));
}

TEST_F(FileSystemTest, isdir) {
  FileSystem().mkdir(test_dir_name());
  ASSERT_TRUE(FileSystem().isdir(test_dir_name()));
  ASSERT_FALSE(FileSystem().isdir(test_file_name()));
  ASSERT_FALSE(FileSystem().isdir(Path("nodir")));
}

TEST_F(FileSystemTest, isreg) {
  ASSERT_TRUE(FileSystem().isreg(test_file_name()));
  FileSystem().mkdir(test_dir_name());
  ASSERT_FALSE(FileSystem().isreg(test_dir_name()));
  ASSERT_FALSE(FileSystem().isreg(Path("nofile.txt")));
}

TEST_F(FileSystemTest, link) {
  if (!FileSystem().link(test_file_name(), test_link_name())) {
    throw Exception();
  }
}

#ifndef _WIN32
TEST_F(FileSystemTest, lstat) {
  if (!FileSystem().symlink(test_file_name(), test_link_name())) {
    throw Exception();
  }
  unique_ptr<Stat> stbuf = FileSystem().lstat(test_link_name());
  ASSERT_TRUE(stbuf->ISLNK());
}
#endif

TEST_F(FileSystemTest, mkdir) {
  if (!FileSystem().mkdir(test_dir_name())) {
    throw Exception();
  }
}

TEST_F(FileSystemTest, mktree) {
  Path test_subdir_path(test_dir_name() / test_dir_name());
  if (!FileSystem().mktree(test_subdir_path)) {
    throw Exception();
  }
  ASSERT_TRUE(FileSystem().exists(test_subdir_path));
}

TEST_F(FileSystemTest, open) {
  unique_ptr<File> file = FileSystem().open(test_file_name());

  unique_ptr<File> no_file = FileSystem().open(Path("nofile.txt"), O_RDONLY);
  if (no_file != NULL) {
    ASSERT_TRUE(false);
  }
}

#ifndef _WIN32
TEST_F(FileSystemTest, readlink) {
  if (!FileSystem().symlink(test_file_name(), test_link_name())) {
    throw Exception();
  }

  Path tarpath;
  if (FileSystem().readlink(test_link_name(), tarpath)) {
    ASSERT_EQ(tarpath, test_file_name());
  } else {
    throw Exception();
  }
}
#endif

TEST_F(FileSystemTest, realpath) {
  Path realpath;
  if (!FileSystem().realpath(test_file_name(), realpath)) {
    throw Exception();
  }
  ASSERT_FALSE(realpath.empty());
  ASSERT_NE(test_file_name(), realpath);
  ASSERT_LT(test_file_name().size(), realpath.size());
}

TEST_F(FileSystemTest, rename) {
  if (!FileSystem().rename(test_file_name(), Path("file_system_test2.txt"))) {
    throw Exception();
  }
  FileSystem().unlink(Path("file_system_test2.txt"));
}

TEST_F(FileSystemTest, rmdir) {
  FileSystem().mkdir(test_dir_name());
  if (!FileSystem().rmdir(test_dir_name())) {
    throw Exception();
  }
}

TEST_F(FileSystemTest, rmtree) {
  if (!FileSystem().mkdir(test_dir_name())) {
    throw Exception();
  }
  if (!FileSystem().mkdir(test_dir_name() / test_dir_name())) {
    throw Exception();
  }
  if (!FileSystem().touch(test_dir_name() / test_file_name())) {
    throw Exception();
  }

  if (!FileSystem().rmtree(test_dir_name())) {
    throw Exception();
  }

  ASSERT_FALSE(FileSystem().exists(test_dir_name()));
}

TEST_F(FileSystemTest, stat) {
  DateTime now = DateTime::now();
  unique_ptr<Stat> stbuf = FileSystem().stat(test_file_name());
  ASSERT_NE(stbuf->atime(), DateTime::INVALID_DATE_TIME);
  ASSERT_LE(stbuf->atime(), now);
  ASSERT_NE(stbuf->ctime(), DateTime::INVALID_DATE_TIME);
  ASSERT_LE(stbuf->ctime(), now);
  ASSERT_NE(stbuf->mtime(), DateTime::INVALID_DATE_TIME);
  ASSERT_LE(stbuf->mtime(), now);
  ASSERT_EQ(stbuf->nlink(), 1);
  ASSERT_EQ(stbuf->size(), 0);
  ASSERT_TRUE(stbuf->ISREG());
}

TEST_F(FileSystemTest, statvfs) {
  FileSystem().mkdir(test_dir_name());
  struct statvfs stbuf;
  if (!FileSystem().statvfs(test_dir_name(), stbuf)) {
    throw Exception();
  }
  ASSERT_GT(stbuf.f_bsize, 0u);
  ASSERT_GT(stbuf.f_blocks, 0);
  ASSERT_GT(stbuf.f_bfree, 0);
  ASSERT_GE(stbuf.f_blocks, stbuf.f_bfree);
}

#ifndef _WIN32
TEST_F(FileSystemTest, symlink) {
  if (!FileSystem().symlink(test_file_name(), test_link_name())) {
    throw Exception();
  }
}
#endif

TEST_F(FileSystemTest, touch) {
  if (!FileSystem().touch(test_file_name())) {
    throw Exception();
  }

  if (FileSystem().touch(test_dir_name() / test_file_name())) {
    ASSERT_TRUE(false);
  }
}

TEST_F(FileSystemTest, unlink) {
  if (!FileSystem().unlink(test_file_name())) {
    throw Exception();
  }
}

TEST_F(FileSystemTest, utime) {
  DateTime atime = DateTime::now(),
           mtime = DateTime::now();

  if (FileSystem().utime(test_file_name(), atime, mtime)) {
    unique_ptr<Stat> stbuf = FileSystem().stat(test_file_name());
    ASSERT_LE(stbuf->atime() - atime, Time::NS_IN_S);
    ASSERT_LE(stbuf->mtime() - mtime, Time::NS_IN_S);
  } else {
    throw Exception();
  }
}

#ifdef _WIN32
TEST_F(FileSystemTest, utime_win32) {
  DateTime atime = DateTime::now();
  DateTime mtime = DateTime::now();
  DateTime ctime = DateTime::now();

  if (FileSystem().utime(test_file_name(), atime, mtime, ctime)) {
    unique_ptr<Stat> stbuf = FileSystem().stat(test_file_name());
    ASSERT_LE(stbuf->atime() - atime, Time::NS_IN_S);
    ASSERT_LE(stbuf->mtime() - mtime, Time::NS_IN_S);
    ASSERT_LE(stbuf->ctime() - ctime, Time::NS_IN_S);
  } else if (Exception::get_last_error_code() != ENOTSUP) {
    throw Exception();
  }
}
#endif
}
}
