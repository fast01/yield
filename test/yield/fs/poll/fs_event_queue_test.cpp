// fs_event_queue_test.cpp

// Copyright (c) 2011 Minor Gordon
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

#include "yield/auto_object.hpp"
#include "yield/assert.hpp"
#include "yield/date_time.hpp"
#include "yield/exception.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/poll/fs_event_queue.hpp"
#include "yunit.hpp"

TEST_SUITE(FSEventQueue);

namespace yield {
namespace fs {
namespace poll {
class FSEventQueueTest : public yunit::Test {
public:
  FSEventQueueTest() {
    test_root_path = Path(".");
    test_directory_path = test_root_path / "dce_test";
    test_file_path = test_root_path / "dce_test.txt";
  }

  // yunit::Test
  void setup() {
    teardown();
  }

  void teardown() {
    FileSystem().rmtree(get_test_directory_path());
    throw_assert_false(FileSystem().exists(get_test_directory_path()));
    FileSystem().unlink(get_test_file_path());
    throw_assert_false(FileSystem().exists(get_test_file_path()));
  }

protected:
  const Path& get_test_directory_path() const {
    return test_directory_path;
  }

  const Path& get_test_file_path() const {
    return test_file_path;
  }

  const Path& get_test_root_path() const {
    return test_root_path;
  }

private:
  Path test_directory_path, test_file_path, test_root_path;
};


TEST_EX(FSEventQueue, associate, FSEventQueueTest) {
  if (!FSEventQueue().associate(get_test_root_path()))
    throw Exception();
}

TEST_EX(FSEventQueue, associate_twice, FSEventQueueTest) {
  FSEventQueue fs_event_queue;

  if (!fs_event_queue.associate(get_test_root_path()))
    throw Exception();

  if (!fs_event_queue.associate(get_test_root_path()))
    throw Exception();

  if (!fs_event_queue.associate(get_test_root_path(), FSEvent::TYPE_FILE_ADD))
    throw Exception();
}

TEST_EX(FSEventQueue, directory_add, FSEventQueueTest) {
  FSEventQueue fs_event_queue;
  if (!fs_event_queue.associate(get_test_root_path()))
    throw Exception();

  if (!FileSystem().mkdir(get_test_directory_path()))
    throw Exception();

  auto_Object<FSEvent> fs_event =
    object_cast<FSEvent>(fs_event_queue.dequeue());
  throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_DIRECTORY_ADD);
  throw_assert_eq(fs_event->get_path(), get_test_directory_path());
}

//TEST_EX(FSEventQueue, directory_add_recursive, FSEventQueueTest) {
//  if (!FileSystem().mkdir(get_test_directory_path()))
//    throw Exception();
//
//  FSEventQueue fs_event_queue;
//  if (!fs_event_queue.associate(get_test_root_path()))
//    throw Exception();
//
//  Path test_subdirectory_path = get_test_directory_path() / "subdir";
//  if (!FileSystem().mkdir(test_subdirectory_path))
//    throw Exception();
//
//  auto_Object<FSEvent> fs_event =
//    object_cast<FSEvent>(fs_event_queue.dequeue());
//  throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_DIRECTORY_ADD);
//  throw_assert_eq(fs_event->get_path(), test_subdirectory_path);
//}

TEST_EX(FSEventQueue, directory_remove, FSEventQueueTest) {
  if (!FileSystem().mkdir(get_test_directory_path()))
    throw Exception();

  FSEventQueue fs_event_queue;
  if (!fs_event_queue.associate(get_test_root_path()))
    throw Exception();

  if (!FileSystem().rmdir(get_test_directory_path()))
    throw Exception();

  auto_Object<FSEvent> fs_event =
    object_cast<FSEvent>(fs_event_queue.dequeue());
  throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_DIRECTORY_REMOVE);
  throw_assert_eq(fs_event->get_path(), get_test_directory_path());
}

//TEST_EX(FSEventQueue, directory_remove_recursive, FSEventQueueTest) {
//  if (!FileSystem().mkdir(get_test_directory_path()))
//    throw Exception();
//
//  Path test_subdirectory_path = get_test_directory_path() / "subdir";
//  if (!FileSystem().mkdir(test_subdirectory_path))
//    throw Exception();
//
//  FSEventQueue fs_event_queue;
//  if (!fs_event_queue.associate(get_test_root_path()))
//    throw Exception();
//
//  if (!FileSystem().rmdir(test_subdirectory_path))
//    throw Exception();
//
//  auto_Object<FSEvent> fs_event =
//    object_cast<FSEvent>(fs_event_queue.dequeue());
//  throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_DIRECTORY_REMOVE);
//  throw_assert_eq(fs_event->get_path(), test_subdirectory_path);
//}

TEST_EX(FSEventQueue, directory_rename, FSEventQueueTest) {
  if (!FileSystem().mkdir(get_test_directory_path()))
    throw Exception();

  Path new_test_directory_path = get_test_root_path() / "test_dir_renamed";
  if (FileSystem().exists(new_test_directory_path))
    if (!FileSystem().rmdir(new_test_directory_path))
      throw Exception();

  FSEventQueue fs_event_queue;
  if (!fs_event_queue.associate(get_test_root_path()))
    throw Exception();

  if (!FileSystem().rename(get_test_directory_path(), new_test_directory_path))
    throw Exception();

  auto_Object<FSEvent> fs_event =
    object_cast<FSEvent>(fs_event_queue.dequeue());
  throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_DIRECTORY_RENAME);
  throw_assert_eq(fs_event->get_old_path(), get_test_directory_path());
  throw_assert_eq(fs_event->get_new_path(), new_test_directory_path);

  FileSystem().rmdir(new_test_directory_path);
}

TEST_EX(FSEventQueue, dissociate, FSEventQueueTest) {
  FSEventQueue fs_event_queue;
  fs_event_queue.associate(get_test_root_path());

  if (!FileSystem().touch(get_test_file_path()))
    throw Exception();

  if (!fs_event_queue.dissociate(get_test_root_path()))
    throw Exception();

  Event* fs_event = fs_event_queue.dequeue(static_cast<uint64_t>(0));
  throw_assert_eq(fs_event, NULL);
}

TEST_EX(FSEventQueue, file_add, FSEventQueueTest) {
  FSEventQueue fs_event_queue;
  if (!fs_event_queue.associate(get_test_root_path()))
    throw Exception();

  if (!FileSystem().touch(get_test_file_path()))
    throw Exception();

  auto_Object<FSEvent> fs_event =
    object_cast<FSEvent>(fs_event_queue.dequeue());
  throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_FILE_ADD);
  throw_assert_eq(fs_event->get_path(), get_test_file_path());
}

//TEST_EX(FSEventQueue, file_add_recursive, FSEventQueueTest) {
//  if (!FileSystem().mkdir(get_test_directory_path()))
//    throw Exception();
//
//  FSEventQueue fs_event_queue;
//  if (!fs_event_queue.associate(get_test_root_path()))
//    throw Exception();
//
//  Path test_file_path = get_test_directory_path() / "file.txt";
//  if (!FileSystem().touch(test_file_path))
//    throw Exception();
//
//  auto_Object<FSEvent> fs_event =
//    object_cast<FSEvent>(fs_event_queue.dequeue());
//  throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_FILE_ADD);
//  throw_assert_eq(fs_event->get_path(), test_file_path);
//}

TEST_EX(FSEventQueue, file_modify, FSEventQueueTest) {
  if (!FileSystem().touch(get_test_file_path()))
    throw Exception();

  FSEventQueue fs_event_queue;
  if (!fs_event_queue.associate(get_test_root_path()))
    throw Exception();

  DateTime atime = DateTime::now(), mtime = atime;
  if (!FileSystem().utime(get_test_file_path(), atime, mtime))
    throw Exception();

  auto_Object<FSEvent> fs_event =
    object_cast<FSEvent>(fs_event_queue.dequeue());
  throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_FILE_MODIFY);
  throw_assert_eq(fs_event->get_path(), get_test_file_path());
}

TEST_EX(FSEventQueue, file_remove, FSEventQueueTest) {
  if (!FileSystem().touch(get_test_file_path()))
    throw Exception();

  FSEventQueue fs_event_queue;
  if (!fs_event_queue.associate(get_test_root_path()))
    throw Exception();

  if (!FileSystem().unlink(get_test_file_path()))
    throw Exception();

  auto_Object<FSEvent> fs_event =
    object_cast<FSEvent>(fs_event_queue.dequeue());
  throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_FILE_REMOVE);
  throw_assert_eq(fs_event->get_path(), get_test_file_path());
}

//TEST_EX(FSEventQueue, file_remove_recursive, FSEventQueueTest) {
//  if (!FileSystem().mkdir(get_test_directory_path()))
//    throw Exception();
//
//  Path test_file_path = get_test_directory_path() / "file.txt";
//  if (!FileSystem().touch(test_file_path))
//    throw Exception();
//
//  FSEventQueue fs_event_queue;
//  if (!fs_event_queue.associate(get_test_root_path()))
//    throw Exception();
//
//  if (!FileSystem().unlink(test_file_path))
//    throw Exception();
//
//  auto_Object<FSEvent> fs_event =
//    object_cast<FSEvent>(fs_event_queue.dequeue());
//  throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_FILE_REMOVE);
//  throw_assert_eq(fs_event->get_path(), test_file_path);
//}

TEST_EX(FSEventQueue, file_rename, FSEventQueueTest) {
  if (!FileSystem().touch(get_test_file_path()))
    throw Exception();

  Path new_test_file_path = get_test_root_path() / "test_file_renamed.txt";
  if (FileSystem().exists(new_test_file_path))
    if (!FileSystem().unlink(new_test_file_path))
      throw Exception();

  FSEventQueue fs_event_queue;
  if (!fs_event_queue.associate(get_test_root_path()))
    throw Exception();

  if (!FileSystem().rename(get_test_file_path(), new_test_file_path))
    throw Exception();

  auto_Object<FSEvent> fs_event =
    object_cast<FSEvent>(fs_event_queue.dequeue());
  throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_FILE_RENAME);
  throw_assert_eq(fs_event->get_old_path(), get_test_file_path());
  throw_assert_eq(fs_event->get_new_path(), new_test_file_path);

  FileSystem().unlink(new_test_file_path);
}
}
}
}
