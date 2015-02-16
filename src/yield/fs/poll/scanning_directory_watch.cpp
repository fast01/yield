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

#include "scanning_directory_watch.hpp"
#include "yield/exception.hpp"
#include "yield/event_handler.hpp"
#include "yield/logging.hpp"
#include "yield/fs/file_system.hpp"

namespace yield {
namespace fs {
namespace poll {
using ::std::make_pair;
using ::std::map;
using ::std::move;
using ::std::pair;
using ::std::shared_ptr;
using ::std::unique_ptr;
using ::std::vector;

ScanningDirectoryWatch::ScanningDirectoryWatch(
  FsEvent::Type fs_event_types,
  const Path& path
) : ScanningWatch(fs_event_types, path),
    dentries_(new map< Path, shared_ptr<Stat> >) {
  unique_ptr<Directory> directory = FileSystem().opendir(this->path());
  if (directory == NULL) {
    throw Exception();
  }
  unique_ptr<Directory::Entry> dentry = directory->read();
  if (dentry == NULL) {
    return;
  }
  for (;;) {
    if (!dentry->is_special()) {
      unique_ptr<Stat> dentry_stat = stat(*dentry);
      if (dentry_stat != NULL) {
        CHECK_EQ(
          dentries_->find(dentry->get_name()),
          dentries_->end()
        );
        dentries_->insert(make_pair(dentry->get_name(), move(dentry_stat)));
      }
    }

    if (!directory->read(*dentry)) {
      break;
    }
  }
}

void
ScanningDirectoryWatch::scan(
  EventHandler<FsEvent>& fs_event_handler,
  FsEvent::Type fs_event_types
) {
  unique_ptr<Directory> directory = FileSystem().opendir(this->path());
  if (directory != NULL) {
    unique_ptr< map<Path, shared_ptr<Stat> > > new_dentries(new map< Path, shared_ptr<Stat> >);
    vector< pair< Path, shared_ptr<Stat> > > new_new_dentries;

    unique_ptr< map< Path, shared_ptr<Stat> > > old_dentries = move(dentries_);

    unique_ptr<Directory::Entry> new_dentry = directory->read();
    if (new_dentry != NULL) {
      for (;;) {
        if (!new_dentry->is_special()) {
          shared_ptr<Stat> new_dentry_stat = stat(*new_dentry);
          if (new_dentry_stat != NULL) {
            CHECK_EQ(
              new_dentries->find(new_dentry->get_name()),
              new_dentries->end()
            );

            new_dentries->insert(
              make_pair(new_dentry->get_name(), new_dentry_stat)
            );

            auto old_dentry_i = old_dentries->find(new_dentry->get_name());
            if (old_dentry_i != old_dentries->end()) {
              shared_ptr<Stat> old_dentry_stat = old_dentry_i->second;
              if (new_dentry->get_type() == type(*old_dentry_stat)) {
                if (!equals(*new_dentry_stat, *old_dentry_stat)) {
                  FsEvent::Type fs_event_type =
                    new_dentry_stat->ISDIR() ?
                    FsEvent::TYPE_DIRECTORY_MODIFY :
                    FsEvent::TYPE_FILE_MODIFY;

                  if ((fs_event_type & fs_event_types) == fs_event_type) {
                    unique_ptr<FsEvent> fs_event(
                    new FsEvent(
                      this->path() / new_dentry->get_name(),
                      fs_event_type
                    ));
                    log_fs_event(*fs_event);
                    fs_event_handler.handle(move(fs_event));
                  }
                }
              } else { // dentry type has changed
                CHECK(false);
              }

              old_dentries->erase(old_dentry_i);
            } else { // old_dentry_i == old_dentries->end()
              // Add the new_dentry to new_new_dentries to check for renaming below
              new_new_dentries.push_back(
                make_pair(new_dentry->get_name(), new_dentry_stat)
              );
            }
          }
        }

        if (!directory->read(*new_dentry)) {
          break;
        }
      }

      // Check the remaining old_dentries against new_new_dentries
      // to match up any renames.
      if (!new_new_dentries.empty()) {
        for (
          auto old_dentry_i = old_dentries->begin();
          old_dentry_i != old_dentries->end();
          ++old_dentry_i
        ) {
          Stat* old_dentry_stat = old_dentry_i->second.get();

          for (
            auto new_dentry_i = new_new_dentries.begin();
            new_dentry_i != new_new_dentries.end();
          ) {
            Stat* new_dentry_stat = new_dentry_i->second.get();

            if (equals(*old_dentry_stat, *new_dentry_stat)) {
              FsEvent::Type fs_event_type =
                new_dentry_stat->ISDIR() ?
                FsEvent::TYPE_DIRECTORY_RENAME :
                FsEvent::TYPE_FILE_RENAME;

              if ((fs_event_type & fs_event_types) == fs_event_type) {
                unique_ptr<FsEvent> fs_event(
                new FsEvent(
                  this->path() / old_dentry_i->first,
                  this->path() / new_dentry_i->first,
                  fs_event_type
                ));
                log_fs_event(*fs_event);
                fs_event_handler.handle(move(fs_event));
              }

              // Don't dec_ref new_dentry_i->second, the Stat;
              // it's owned by new_dentries
              new_dentry_i = new_new_dentries.erase(new_dentry_i);
              old_dentry_i->second.reset();
              break;
            } else {
              ++new_dentry_i;
            }
          }
        }

        // Any remaining new_new_dentries here are adds.
        if (
          want_fs_event_type(FsEvent::TYPE_DIRECTORY_ADD)
          ||
          want_fs_event_type(FsEvent::TYPE_FILE_ADD)
        ) {
          for (
            auto new_dentry_i = new_new_dentries.begin();
            new_dentry_i != new_new_dentries.end();
            ++new_dentry_i
          ) {
            FsEvent::Type fs_event_type
            = new_dentry_i->second->ISDIR() ?
              FsEvent::TYPE_DIRECTORY_ADD :
              FsEvent::TYPE_FILE_ADD;

            if ((fs_event_type & fs_event_types) == fs_event_type) {
              unique_ptr<FsEvent> fs_event(
              new FsEvent(
                this->path() / new_dentry_i->first,
                fs_event_type
              ));
              log_fs_event(*fs_event);
              fs_event_handler.handle(move(fs_event));
            }
          }
        }
      }

      // Any remaining old_dentries here are removes.
      for (
        auto old_dentry_i = old_dentries->begin();
        old_dentry_i != old_dentries->end();
        ++old_dentry_i
      ) {
        if (old_dentry_i->second == NULL) {
          continue;
        }

        FsEvent::Type fs_event_type
        = old_dentry_i->second->ISDIR() ?
          FsEvent::TYPE_DIRECTORY_REMOVE :
          FsEvent::TYPE_FILE_REMOVE;

        if ((fs_event_type & fs_event_types) == fs_event_type) {
          unique_ptr<FsEvent> fs_event(
          new FsEvent(
            this->path() / old_dentry_i->first,
            fs_event_type
          ));
          log_fs_event(*fs_event);
          fs_event_handler.handle(move(fs_event));
        }
      }
    }

    dentries_.reset(new_dentries.release());
  } else { // directory == NULL
    for (
      auto dentry_i = dentries_->begin();
      dentry_i != dentries_->end();
      ++dentry_i
    ) {
      FsEvent::Type fs_event_type =
        dentry_i->second->ISDIR() ?
        FsEvent::TYPE_DIRECTORY_REMOVE :
        FsEvent::TYPE_FILE_REMOVE;

      if ((fs_event_type & fs_event_types) == fs_event_type) {
        unique_ptr<FsEvent> fs_event(
        new FsEvent(
          this->path() / dentry_i->first,
          fs_event_type
        ));
        log_fs_event(*fs_event);
        fs_event_handler.handle(move(fs_event));
      }
    }

    dentries_.reset();
  }
}

unique_ptr<Stat> ScanningDirectoryWatch::stat(Directory::Entry& dentry) {
#ifdef _WIN32
  return unique_ptr<Stat>(new Stat(dentry));
#else
  return FileSystem().stat(this->path() / dentry.get_name());
#endif
}
}
}
}
