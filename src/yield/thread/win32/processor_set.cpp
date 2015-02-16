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

#include "yield/thread/processor_set.hpp"

#include <Windows.h>

namespace yield {
namespace thread {
ProcessorSet::ProcessorSet() {
  mask = 0;
}

ProcessorSet::~ProcessorSet() {
}

void ProcessorSet::clear() {
  mask = 0;
}

void ProcessorSet::clear(uint16_t processor_i) {
  mask &= ~(1L << processor_i);
}

uint16_t ProcessorSet::get_online_logical_processor_count() {
  SYSTEM_INFO system_info;
  GetSystemInfo(&system_info);
  return static_cast<uint16_t>(system_info.dwNumberOfProcessors);
}

uint16_t ProcessorSet::get_online_physical_processor_count() {
  SYSTEM_INFO system_info;
  GetSystemInfo(&system_info);
  return static_cast<uint16_t>(system_info.dwNumberOfProcessors);
}

bool ProcessorSet::isset(uint16_t processor_i) const {
  if (processor_i < sizeof(uintptr_t) * 8) {
    uintptr_t bit = (1UL << processor_i);
    return (bit & mask) == bit;
  } else {
    return false;
  }
}

bool ProcessorSet::set(uint16_t processor_i) {
  mask |= (1UL << processor_i);
  return true;
}
}
}
