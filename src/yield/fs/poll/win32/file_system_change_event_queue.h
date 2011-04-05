/* yield/fs/poll/win32/file_system_change_event_queue.h */

/* Copyright (c) 2011 Minor Gordon */
/* All rights reserved */

/* This source file is part of the Yield project. */

/* Redistribution and use in source and binary forms, with or without */
/* modification, are permitted provided that the following conditions are met: */
/* * Redistributions of source code must retain the above copyright */
/* notice, this list of conditions and the following disclaimer. */
/* * Redistributions in binary form must reproduce the above copyright */
/* notice, this list of conditions and the following disclaimer in the */
/* documentation and/or other materials provided with the distribution. */
/* * Neither the name of the Yield project nor the */
/* names of its contributors may be used to endorse or promote products */
/* derived from this software without specific prior written permission. */

/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" */
/* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE */
/* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE */
/* ARE DISCLAIMED. IN NO EVENT SHALL Minor Gordon BE LIABLE FOR ANY DIRECT, */
/* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES */
/* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; */
/* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND */
/* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT */
/* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF */
/* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#ifndef _YIELD_FS_POLL_WIN32_FILE_SYSTEM_CHANGE_EVENT_QUEUE_H_
#define _YIELD_FS_POLL_WIN32_FILE_SYSTEM_CHANGE_EVENT_QUEUE_H_

#ifndef _YIELD_FS_POLL_WIN32_FILE_SYSTEM_CHANGE_EVENT_QUEUE_H_
#define _YIELD_FS_POLL_WIN32_FILE_SYSTEM_CHANGE_EVENT_QUEUE_H_

#include "yield/fs/volume_change_event_queue.h"
#include "yield/poll/win32/io_completion_port.h"


namespace yield
{
  namespace fs
  {
    namespace win32
    {
      using yield::poll::win32::IOCompletionPort;


      class DirectoryChangeEventQueue : public VolumeChangeEventQueue
      {
      public:
        ~DirectoryChangeEventQueue();

        static YO_NEW_REF DirectoryChangeEventQueue& create();

        // VolumeChangeEventQueue
        bool associate
        (
          const Path& path,
          VolumeChangeEvent::Type events,
          Flag flags
        );

        void dissociate( const Path& path );

        int
        poll
        (
          VolumeChangeEvent* volume_change_events,
          int volume_change_events_len,
          const Time& timeout
        );

      private:
        class Watch;

      private:
        DirectoryChangeEventQueue( IOCompletionPort& io_completion_port );

      private:
        IOCompletionPort& io_completion_port;
        WatchMap<Watch> watches;
      };
    };
  };
};

#endif
