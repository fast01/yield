// yield/poll/fd_event_queue.hpp

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

#ifndef _YIELD_POLL_FD_EVENT_QUEUE_HPP_
#define _YIELD_POLL_FD_EVENT_QUEUE_HPP_

#include "yield/poll/fd_event.hpp"

#if defined(__FreeBSD__) || defined(__MACH__) || defined(__OpenBSD__)
#include "yield/poll/bsd/fd_event_queue.hpp"
#elif defined(__linux__)
#include "yield/poll/linux/fd_event_queue.hpp"
#elif defined(__sun)
#include "yield/poll/sunos/fd_event_queue.hpp"
#elif defined(_WIN32)
#include "yield/poll/win32/fd_event_queue.hpp"
#else
#include "yield/poll/posix/fd_event_queue.hpp"
#endif

namespace yield {
namespace poll {
#if defined(__FreeBSD__) || defined(__MACH__) || defined(__OpenBSD__)
typedef bsd::FDEventQueue FDEventQueue;
#elif defined(__linux__)
typedef linux::FDEventQueue FDEventQueue;
#elif defined(__sun)
typedef sunos::FDEventQueue FDEventQueue;
#elif defined(_WIN32)
typedef win32::FDEventQueue FDEventQueue;
#else
typedef posix::FDEventQueue FDEventQueue;
#endif
}
}

#endif
