// yield_fs_poll_test_main.cpp

// Copyright (c) 2012 Minor Gordon
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

#include "yunit.hpp"

#include <iostream>

#if defined(__MACH__) || defined(__FreeBSD__)
extern yunit::TestSuite& BSDFSEventQueueTestSuite();
#endif
extern yunit::TestSuite& FSEventTestSuite();
#ifdef __linux__
extern yunit::TestSuite& LinuxFSEventQueueTestSuite();
#endif
extern yunit::TestSuite& ScanningFSEventQueueTestSuite();
#ifdef _WIN32
extern yunit::TestSuite& Win32FSEventQueueTestSuite();
#endif

int main(int, char**) {
  int failed_test_case_count = 0;

#if defined(__MACH__) || defined(__FreeBSD__)
  // BSDFSEventQueue
  std::cout << "BSDFSEventQueue:" << std::endl;
  failed_test_case_count += BSDFSEventQueueTestSuite().run();
  std::cout << std::endl;
#endif

  // FSEvent
  std::cout << "FSEvent:" << std::endl;
  failed_test_case_count += FSEventTestSuite().run();
  std::cout << std::endl;

#ifdef __linux__
  // LinuxFSEventQueue
  std::cout << "LinuxFSEventQueue:" << std::endl;
  failed_test_case_count += LinuxFSEventQueueTestSuite().run();
  std::cout << std::endl;
#endif

  // ScanningFSEventQueue
  std::cout << "ScanningFSEventQueue:" << std::endl;
  failed_test_case_count += ScanningFSEventQueueTestSuite().run();
  std::cout << std::endl;

#ifdef _WIN32
  // Win32FSEventQueue
  std::cout << "Win32FSEventQueue:" << std::endl;
  failed_test_case_count += Win32FSEventQueueTestSuite().run();
  std::cout << std::endl;
#endif

  return failed_test_case_count;
}
