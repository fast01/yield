import os.path


PLATFORM_NAMES = ('bsd', 'darwin', 'linux', 'posix', 'sunos', 'unix', 'win32')
ROOT_DIR_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
INCLUDE_DIR_PATH = os.path.join(ROOT_DIR_PATH, 'include')


includes = []
for include_dir_path, _, include_file_names in os.walk(INCLUDE_DIR_PATH):
	include_dir_name = os.path.split(include_dir_path)[1]
	if include_dir_path == INCLUDE_DIR_PATH:
		continue
	elif include_dir_name in PLATFORM_NAMES:
		continue
	for include_file_name in include_file_names:
		include_file_ext = os.path.splitext(include_file_name)[1]
		if include_file_ext != '.hpp':
			continue
		include_file_path = os.path.join(include_dir_path, include_file_name)
		include_file_relpath = os.path.relpath(include_file_path, INCLUDE_DIR_PATH)
		include_file_relpath_posix = include_file_relpath.replace(os.path.sep, '/')
		if include_file_relpath_posix == 'yield/logging.hpp':
			continue
		includes.append('#include "%s"' % include_file_relpath_posix)
includes.sort()
includes = "\n".join(includes)
with open(os.path.join(INCLUDE_DIR_PATH, 'yield.hpp'), 'w+') as f:
	f.write("""\
#ifndef _YIELD_HPP_
#define _YIELD_HPP_

%(includes)s

#endif  // _YIELD_HPP_
""" % locals())
