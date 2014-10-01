// yield/http/server/file/http_file_request_handler.cpp

// Copyright (c) 2013 Minor Gordon
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

#include "yield/logging.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/http/server/http_request.hpp"
#include "yield/http/server/file/http_file_request_handler.hpp"

namespace yield {
namespace http {
namespace server {
namespace file {
using yield::fs::File;
using yield::fs::FileSystem;
using yield::fs::Path;
using yield::uri::Uri;

const static char* ETLUA = "local VERSION = \"1.0.2\"\r\nlocal insert, concat\r\ndo\r\n  local _obj_0 = table\r\n  insert, concat = _obj_0.insert, _obj_0.concat\r\nend\r\nlocal load, setfenv, assert, type, error, tostring, tonumber, setmetatable\r\ndo\r\n  local _obj_0 = _G\r\n  load, setfenv, assert, type, error, tostring, tonumber, setmetatable = _obj_0.load, _obj_0.setfenv, _obj_0.assert, _obj_0.type, _obj_0.error, _obj_0.tostring, _obj_0.tonumber, _obj_0.setmetatable\r\nend\r\nsetfenv = setfenv or function(fn, env)\r\n  local name\r\n  local i = 1\r\n  while true do\r\n    name = debug.getupvalue(fn, i)\r\n    if not name or name == \"_ENV\" then\r\n      break\r\n    end\r\n    i = i + 1\r\n  end\r\n  if name then\r\n    debug.upvaluejoin(fn, i, (function()\r\n      return env\r\n    end), 1)\r\n  end\r\n  return fn\r\nend\r\nlocal html_escape_entities = {\r\n  ['&'] = '&amp;',\r\n  ['<'] = '&lt;',\r\n  ['>'] = '&gt;',\r\n  ['\"'] = '&quot;',\r\n  [\"'\"] = '&#039;'\r\n}\r\nlocal html_escape\r\nhtml_escape = function(str)\r\n  return (str:gsub([=[[\"><'&]]=], html_escape_entities))\r\nend\r\nlocal get_line\r\nget_line = function(str, line_num)\r\n  for line in str:gmatch(\"([^\\n]*)\\n?\") do\r\n    if line_num == 1 then\r\n      return line\r\n    end\r\n    line_num = line_num - 1\r\n  end\r\nend\r\nlocal pos_to_line\r\npos_to_line = function(str, pos)\r\n  local line = 1\r\n  for _ in str:sub(1, pos):gmatch(\"\\n\") do\r\n    line = line + 1\r\n  end\r\n  return line\r\nend\r\nlocal Compiler\r\ndo\r\n  local _base_0 = {\r\n    render = function(self)\r\n      return table.concat(self.buffer)\r\n    end,\r\n    push = function(self, str, ...)\r\n      local i = self.i + 1\r\n      self.buffer[i] = str\r\n      self.i = i\r\n      if ... then\r\n        return self:push(...)\r\n      end\r\n    end,\r\n    header = function(self)\r\n      return self:push(\"local _tostring, _escape, _b, _b_i = ...\\n\")\r\n    end,\r\n    footer = function(self)\r\n      return self:push(\"return _b\")\r\n    end,\r\n    increment = function(self)\r\n      return self:push(\"_b_i = _b_i + 1\\n\")\r\n    end,\r\n    mark = function(self, pos)\r\n      return self:push(\"--[[\", tostring(pos), \"]] \")\r\n    end,\r\n    assign = function(self, ...)\r\n      self:push(\"_b[_b_i] = \", ...)\r\n      if ... then\r\n        return self:push(\"\\n\")\r\n      end\r\n    end\r\n  }\r\n  _base_0.__index = _base_0\r\n  local _class_0 = setmetatable({\r\n    __init = function(self)\r\n      self.buffer = { }\r\n      self.i = 0\r\n    end,\r\n    __base = _base_0,\r\n    __name = \"Compiler\"\r\n  }, {\r\n    __index = _base_0,\r\n    __call = function(cls, ...)\r\n      local _self_0 = setmetatable({}, _base_0)\r\n      cls.__init(_self_0, ...)\r\n      return _self_0\r\n    end\r\n  })\r\n  _base_0.__class = _class_0\r\n  Compiler = _class_0\r\nend\r\nlocal Parser\r\ndo\r\n  local _base_0 = {\r\n    open_tag = \"<%\",\r\n    close_tag = \"%>\",\r\n    modifiers = \"^[=-]\",\r\n    html_escape = true,\r\n    next_tag = function(self)\r\n      local start, stop = self.str:find(self.open_tag, self.pos, true)\r\n      if not (start) then\r\n        self:push_raw(self.pos, #self.str)\r\n        return false\r\n      end\r\n      if not (start == self.pos) then\r\n        self:push_raw(self.pos, start - 1)\r\n      end\r\n      self.pos = stop + 1\r\n      local modifier\r\n      if self.str:match(self.modifiers, self.pos) then\r\n        do\r\n          local _with_0 = self.str:sub(self.pos, self.pos)\r\n          self.pos = self.pos + 1\r\n          modifier = _with_0\r\n        end\r\n      end\r\n      local close_start, close_stop = self.str:find(self.close_tag, self.pos, true)\r\n      if not (close_start) then\r\n        return nil, self:error_for_pos(start, \"failed to find closing tag\")\r\n      end\r\n      while self:in_string(self.pos, close_start) do\r\n        close_start, close_stop = self.str:find(self.close_tag, close_stop, true)\r\n      end\r\n      local trim_newline\r\n      if \"-\" == self.str:sub(close_start - 1, close_start - 1) then\r\n        close_start = close_start - 1\r\n        trim_newline = true\r\n      end\r\n      self:push_code(modifier or \"code\", self.pos, close_start - 1)\r\n      self.pos = close_stop + 1\r\n      if trim_newline then\r\n        do\r\n          local match = self.str:match(\"^\\n\", self.pos)\r\n          if match then\r\n            self.pos = self.pos + #match\r\n          end\r\n        end\r\n      end\r\n      return true\r\n    end,\r\n    in_string = function(self, start, stop)\r\n      local in_string = false\r\n      local end_delim = nil\r\n      local escape = false\r\n      local pos = 0\r\n      local skip_until = nil\r\n      local chunk = self.str:sub(start, stop)\r\n      for char in chunk:gmatch(\".\") do\r\n        local _continue_0 = false\r\n        repeat\r\n          pos = pos + 1\r\n          if skip_until then\r\n            if pos <= skip_until then\r\n              _continue_0 = true\r\n              break\r\n            end\r\n            skip_until = nil\r\n          end\r\n          if end_delim then\r\n            if end_delim == char and not escape then\r\n              in_string = false\r\n              end_delim = nil\r\n            end\r\n          else\r\n            if char == \"'\" or char == '\"' then\r\n              end_delim = char\r\n              in_string = true\r\n            end\r\n            if char == \"[\" then\r\n              do\r\n                local lstring = chunk:match(\"^%[=*%[\", pos)\r\n                if lstring then\r\n                  local lstring_end = lstring:gsub(\"%[\", \"]\")\r\n                  local lstring_p1, lstring_p2 = chunk:find(lstring_end, pos, true)\r\n                  if not (lstring_p1) then\r\n                    return true\r\n                  end\r\n                  skip_until = lstring_p2\r\n                end\r\n              end\r\n            end\r\n          end\r\n          escape = char == \"\\\\\"\r\n          _continue_0 = true\r\n        until true\r\n        if not _continue_0 then\r\n          break\r\n        end\r\n      end\r\n      return in_string\r\n    end,\r\n    push_raw = function(self, start, stop)\r\n      return insert(self.chunks, self.str:sub(start, stop))\r\n    end,\r\n    push_code = function(self, kind, start, stop)\r\n      return insert(self.chunks, {\r\n        kind,\r\n        self.str:sub(start, stop),\r\n        start\r\n      })\r\n    end,\r\n    compile = function(self, str)\r\n      local success, err = self:parse(str)\r\n      if not (success) then\r\n        return nil, err\r\n      end\r\n      local fn\r\n      fn, err = self:load(self:chunks_to_lua())\r\n      if not (fn) then\r\n        return nil, err\r\n      end\r\n      return function(...)\r\n        local buffer\r\n        buffer, err = self:run(fn, ...)\r\n        if buffer then\r\n          return concat(buffer)\r\n        else\r\n          return nil, err\r\n        end\r\n      end\r\n    end,\r\n    parse = function(self, str)\r\n      self.str = str\r\n      assert(type(self.str) == \"string\", \"expecting string for parse\")\r\n      self.pos = 1\r\n      self.chunks = { }\r\n      while true do\r\n        local found, err = self:next_tag()\r\n        if err then\r\n          return nil, err\r\n        end\r\n        if not (found) then\r\n          break\r\n        end\r\n      end\r\n      return true\r\n    end,\r\n    parse_error = function(self, err, code)\r\n      local line_no, err_msg = err:match(\"%[.-%]:(%d+): (.*)$\")\r\n      line_no = tonumber(line_no)\r\n      if not (line_no) then\r\n        return \r\n      end\r\n      local line = get_line(code, line_no)\r\n      local source_pos = tonumber(line:match(\"^%-%-%[%[(%d+)%]%]\"))\r\n      if not (source_pos) then\r\n        return \r\n      end\r\n      return self:error_for_pos(source_pos, err_msg)\r\n    end,\r\n    error_for_pos = function(self, source_pos, err_msg)\r\n      local source_line_no = pos_to_line(self.str, source_pos)\r\n      local source_line = get_line(self.str, source_line_no)\r\n      return tostring(err_msg) .. \" [\" .. tostring(source_line_no) .. \"]: \" .. tostring(source_line)\r\n    end,\r\n    load = function(self, code, name)\r\n      if name == nil then\r\n        name = \"etlua\"\r\n      end\r\n      local code_fn\r\n      do\r\n        local code_ref = code\r\n        code_fn = function()\r\n          do\r\n            local ret = code_ref\r\n            code_ref = nil\r\n            return ret\r\n          end\r\n        end\r\n      end\r\n      local fn, err = load(code_fn, name)\r\n      if not (fn) then\r\n        do\r\n          local err_msg = self:parse_error(err, code)\r\n          if err_msg then\r\n            return nil, err_msg\r\n          end\r\n        end\r\n        return nil, err\r\n      end\r\n      return fn\r\n    end,\r\n    run = function(self, fn, env, buffer, i, ...)\r\n      if env == nil then\r\n        env = { }\r\n      end\r\n      local combined_env = setmetatable({ }, {\r\n        __index = function(self, name)\r\n          local val = env[name]\r\n          if val == nil then\r\n            val = _G[name]\r\n          end\r\n          return val\r\n        end\r\n      })\r\n      if not (buffer) then\r\n        buffer = { }\r\n        i = 0\r\n      end\r\n      setfenv(fn, combined_env)\r\n      return fn(tostring, html_escape, buffer, i, ...)\r\n    end,\r\n    compile_to_lua = function(self, str, ...)\r\n      local success, err = self:parse(str)\r\n      if not (success) then\r\n        return nil, err\r\n      end\r\n      return self:chunks_to_lua(...)\r\n    end,\r\n    chunks_to_lua = function(self, compiler_cls)\r\n      if compiler_cls == nil then\r\n        compiler_cls = Compiler\r\n      end\r\n      local r = compiler_cls()\r\n      r:header()\r\n      local _list_0 = self.chunks\r\n      for _index_0 = 1, #_list_0 do\r\n        local chunk = _list_0[_index_0]\r\n        local t = type(chunk)\r\n        if t == \"table\" then\r\n          t = chunk[1]\r\n        end\r\n        local _exp_0 = t\r\n        if \"string\" == _exp_0 then\r\n          r:increment()\r\n          r:assign((\"%q\"):format(chunk))\r\n        elseif \"code\" == _exp_0 then\r\n          r:mark(chunk[3])\r\n          r:push(chunk[2], \"\\n\")\r\n        elseif \"=\" == _exp_0 or \"-\" == _exp_0 then\r\n          r:increment()\r\n          r:mark(chunk[3])\r\n          r:assign()\r\n          if t == \"=\" and self.html_escape then\r\n            r:push(\"_escape(_tostring(\", chunk[2], \"))\\n\")\r\n          else\r\n            r:push(\"_tostring(\", chunk[2], \")\\n\")\r\n          end\r\n        else\r\n          error(\"unknown type \" .. tostring(t))\r\n        end\r\n      end\r\n      r:footer()\r\n      return r:render()\r\n    end\r\n  }\r\n  _base_0.__index = _base_0\r\n  local _class_0 = setmetatable({\r\n    __init = function() end,\r\n    __base = _base_0,\r\n    __name = \"Parser\"\r\n  }, {\r\n    __index = _base_0,\r\n    __call = function(cls, ...)\r\n      local _self_0 = setmetatable({}, _base_0)\r\n      cls.__init(_self_0, ...)\r\n      return _self_0\r\n    end\r\n  })\r\n  _base_0.__class = _class_0\r\n  Parser = _class_0\r\nend\r\nlocal compile\r\ndo\r\n  local _base_0 = Parser()\r\n  local _fn_0 = _base_0.compile\r\n  compile = function(...)\r\n    return _fn_0(_base_0, ...)\r\n  end\r\nend\r\nlocal render\r\nrender = function(str, ...)\r\n  local fn, err = compile(str)\r\n  if fn then\r\n    return fn(...)\r\n  else\r\n    return nil, err\r\n  end\r\nend\r\nreturn {\r\n  compile = compile,\r\n  render = render,\r\n  Parser = Parser,\r\n  Compiler = Compiler,\r\n  _version = VERSION\r\n}\r\n";


HttpFileRequestHandler::HttpFileRequestHandler(
  const Path& root_directory_path,
  const Uri& root_uri
) : root_directory_path(root_directory_path),
  root_uri(root_uri) {
  this->root_uri.get_path(root_uri_path);
}

void HttpFileRequestHandler::handle(YO_NEW_REF Event& event) {
  CHECK_EQ(event.get_type_id(), HttpRequest::TYPE_ID);
  HttpRequest& http_request = static_cast<HttpRequest&>(event);

  iovec http_request_uri_path;
  http_request.get_uri().get_path(http_request_uri_path);

  if (http_request_uri_path.iov_len > root_uri_path.iov_len) {
    if (
      memcmp(
        http_request_uri_path.iov_base,
        root_uri_path.iov_base,
        root_uri_path.iov_len
      ) == 0
    ) {
      Path file_relpath(
        static_cast<char*>(http_request_uri_path.iov_base) +
        root_uri_path.iov_len,
        http_request_uri_path.iov_len - root_uri_path.iov_len
      );

      Path file_abspath = root_directory_path / file_relpath;

      File* file = FileSystem().open(file_abspath);
      if (file != NULL) {
        File::Map* map = file->mmap(SIZE_MAX, 0, true);
        if (map != NULL) {
          http_request.respond(200, *map);
        } else {
          http_request.respond(404, Exception());
        }

        File::dec_ref(*file);
      } else {
        http_request.respond(404, Exception());
      }

      HttpRequest::dec_ref(http_request);
    }
  }
}
}
}
}
}
