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

#include "yield/thread/processor_set.hpp"

#include <unistd.h>

namespace yield {
namespace thread {
#pragma GCC diagnostic ignored "-Wold-style-cast"
ProcessorSet::ProcessorSet() {
  CPU_ZERO(&cpu_set);
}

ProcessorSet::~ProcessorSet() {
}

void ProcessorSet::clear() {
  CPU_ZERO(&cpu_set);
}

void ProcessorSet::clear(uint16_t processor_i) {
  CPU_CLR(processor_i, &cpu_set);
}

uint16_t ProcessorSet::get_online_logical_processor_count() {
  return static_cast<uint16_t>(sysconf(_SC_NPROCESSORS_ONLN));
}

uint16_t ProcessorSet::get_online_physical_processor_count() {
  return static_cast<uint16_t>(sysconf(_SC_NPROCESSORS_ONLN));
}

bool ProcessorSet::isset(uint16_t processor_i) const {
  return CPU_ISSET(processor_i, &cpu_set);
}

bool ProcessorSet::set(uint16_t processor_i) {
  CPU_SET(processor_i, &cpu_set);
  return true;
}
#pragma GCC diagnostic warning "-Wold-style-cast"
}
}
