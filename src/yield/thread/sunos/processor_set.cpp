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

#include <kstat.h> // For kstat
#include <sys/processor.h> // For p_online
#include <sys/pset.h>

namespace yield {
namespace thread {
ProcessorSet::ProcessorSet() {
  psetid = PS_NONE; // Don't pset_create until we actually use the set,
  // to avoid leaving state in the system
}

ProcessorSet::~ProcessorSet() {
  if (psetid != PS_NONE) {
    pset_destroy(psetid);
  }
}

void ProcessorSet::clear() {
  if (psetid != PS_NONE) {
    pset_destroy(psetid);
    psetid = PS_NONE;
  }
}

uint16_t ProcessorSet::get_online_logical_processor_count() {
  uint16_t online_logical_processor_count = 0;

  processorid_t cpuid_max = sysconf(_SC_CPUID_MAX);
  for (processorid_t cpuid_i = 0; cpuid_i <= cpuid_max; cpuid_i++) {
    if (p_online(cpuid_i, P_STATUS) == P_ONLINE) {
      online_logical_processor_count++;
    }
  }

  return online_logical_processor_count;
}

uint16_t ProcessorSet::get_online_physical_processor_count() {
  kstat_ctl_t* kc = kstat_open();
  if (kc) {
    uint16_t online_physical_processor_count = 1;

    kstat* ksp = kstat_lookup(kc, "cpu_info", -1, NULL);
    int32_t last_core_id = 0;
    while (ksp) {
      kstat_read(kc, ksp, NULL);
      kstat_named_t* knp;
      knp = (kstat_named_t*)kstat_data_lookup(ksp, "core_id");
      if (knp) {
        int32_t this_core_id = knp->value.i32;
        if (this_core_id != last_core_id) {
          online_physical_processor_count++;
          last_core_id = this_core_id;
        }
      }
      ksp = ksp->ks_next;
    }

    kstat_close(kc);

    return online_physical_processor_count;
  } else {
    return 1;
  }
}

void ProcessorSet::clear(uint16_t processor_i) {
  if (psetid != PS_NONE) {
    pset_assign(PS_NONE, processor_i, NULL);
  }
}

bool ProcessorSet::isset(uint16_t processor_i) const {
  if (psetid != PS_NONE) {
    psetid_t check_psetid;
    return pset_assign(
             PS_QUERY,
             processor_i,
             &check_psetid
           ) == 0
           &&
           check_psetid == psetid;
  }
}

bool ProcessorSet::set(uint16_t processor_i) {
  if (psetid == PS_NONE) {
    if (pset_create(&psetid) != 0) {
      return false;
    }
  }

  return pset_assign(psetid, processor_i, NULL) == 0;
}
}
}
