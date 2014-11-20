import os.path


PLATFORM_NAMES = ('bsd', 'darwin', 'linux', 'posix', 'sunos', 'unix', 'win32')
ROOT_DIR_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))


globs = []
darwin_add_library_files = []
freebsd_add_library_files = []
linux_add_library_files = []
sunos_add_library_files = []
win32_add_library_files = []
win32_source_groups = []
win32_set_source_file_properties = []
for src_dir_name, source_group_name in (('include', 'Header Files'), ('src', 'Source Files')):
	# src_dir_name_upper = src_dir_name.upper()
	src_dir_path = os.path.join(ROOT_DIR_PATH, src_dir_name)
	for dir_path, subdir_names, file_names in os.walk(src_dir_path):
		dir_name = os.path.split(dir_path)[1]
		dir_relpath = os.path.relpath(dir_path, ROOT_DIR_PATH)
		dir_relpath_posix = dir_relpath.replace(os.path.sep, '/')
		dir_relpath_win32 = dir_relpath.replace(os.path.sep, '\\')
		dir_relpath_ = dir_relpath.replace(os.path.sep, '_').upper().rstrip('.')
		if dir_path == src_dir_path:
			dir_relpath_ += '_'
		globs.append("""file(GLOB %(dir_relpath_)s "%(dir_relpath_posix)s/*.h" "%(dir_relpath_posix)s/*.hpp" "%(dir_relpath_posix)s/*.cpp" "%(dir_relpath_posix)s/*.h" "%(dir_relpath_posix)s/*.rl")""" % locals())
		add_library_file = "\t\t${%(dir_relpath_)s}" % locals()
		if dir_name in PLATFORM_NAMES:
			if dir_name == 'bsd':
				darwin_add_library_files.append(add_library_file)
				freebsd_add_library_files.append(add_library_file)
			elif dir_name == 'darwin':
				darwin_add_library_files.append(add_library_file)
			elif dir_name == 'linux':
				linux_add_library_files.append(add_library_file)
			elif dir_name == 'posix':
				darwin_add_library_files.append(add_library_file)
				linux_add_library_files.append(add_library_file)
				sunos_add_library_files.append(add_library_file)
			elif dir_name == 'sunos':
				sunos_add_library_files.append(add_library_file)
			elif dir_name == 'unix':
				darwin_add_library_files.append(add_library_file)
				linux_add_library_files.append(add_library_file)
				sunos_add_library_files.append(add_library_file)
			elif dir_name == 'win32':
				pass
			else:
				raise NotImplementedError(dir_name)
		else:
			darwin_add_library_files.append(add_library_file)
			freebsd_add_library_files.append(add_library_file)
			linux_add_library_files.append(add_library_file)
			sunos_add_library_files.append(add_library_file)
		win32_add_library_files.append(add_library_file)
		if dir_name in PLATFORM_NAMES and dir_name != 'win32':
			win32_set_source_file_properties.append("\tset_source_files_properties(${%(dir_relpath_)s} PROPERTIES HEADER_FILE_ONLY true)" % locals())
		win32_source_groups.append("""\tsource_group("%s\\\\%s" FILES ${%s})""" % (source_group_name, dir_relpath_win32[len(src_dir_name) + 1:].replace('\\', '\\\\'), dir_relpath_))
darwin_add_library_files = "\n".join(darwin_add_library_files)
globs = "\n".join(globs)
freebsd_add_library_files = "\n".join(freebsd_add_library_files)
linux_add_library_files = "\n".join(linux_add_library_files)
sunos_add_library_files = "\n".join(sunos_add_library_files)
win32_add_library_files = "\n".join(win32_add_library_files)
win32_set_source_file_properties = "\n".join(win32_set_source_file_properties)
win32_source_groups = "\n".join(win32_source_groups)
with open(os.path.join(ROOT_DIR_PATH, 'YieldLib.cmake'), 'w+') as f:
	f.write("""\
include(YieldFlags.cmake)

%(globs)s
if (WIN32)
%(win32_set_source_file_properties)s
%(win32_source_groups)s
	add_library(
		yield STATIC
%(win32_add_library_files)s
	)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	add_library(
		yield STATIC
%(darwin_add_library_files)s
	)
	target_link_libraries(yield pthread)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
	add_library(
		yield STATIC
%(freebsd_add_library_files)s
	)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_library(
		yield STATIC
%(linux_add_library_files)s
	)
	target_link_libraries(yield pthread)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
	add_library(
		yield STATIC
%(sunos_add_library_files)s
	)
	target_link_libraries(yield kstat)
endif()
set_target_properties(yield PROPERTIES LINKER_LANGUAGE CXX)
""" % locals())

