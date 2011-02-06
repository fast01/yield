// yield/fs/posix/extended_attributes.hpp

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

#ifndef _YIELD_FS_POSIX_EXTENDED_ATTRIBUTES_HPP_
#define _YIELD_FS_POSIX_EXTENDED_ATTRIBUTES_HPP_

#include "yield/object.hpp"


namespace yield {
namespace fs {
class Path;


namespace posix {

class ExtendedAttributes : public Object {
public:
  virtual ~ExtendedAttributes() { }

  virtual bool get(const char* name, string& value);
  virtual ssize_t get(const char* name, void* value, size_t size) = 0;
  virtual bool list(vector<string>& out_names) = 0;
  virtual bool remove(const char* name) = 0;

  virtual bool set(const char* name, const char* value, int flags = 0);
  virtual bool set(const char* name, const string& value, int flags = 0);

  virtual bool
  set
  (
    const char* name,
    const void* value,
    size_t size,
    int flags = 0
  ) = 0;

  // Object
  ExtendedAttributes& inc_ref() {
    return Object::inc_ref(*this);
  }
};
}
}
}

#endif