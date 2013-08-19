import os.path


PLATFORM_NAMES = ('bsd', 'darwin', 'linux', 'posix', 'sunos', 'unix', 'win32')
ROOT_DIR_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))


add_library_files = []
globs = []
source_groups = []
set_source_file_properties = []
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
		add_library_files.append("\t${%(dir_relpath_)s}" % locals())
		if dir_name in PLATFORM_NAMES and dir_name != 'win32':
			set_source_file_properties.append("\tset_source_files_properties(${%(dir_relpath_)s} PROPERTIES HEADER_FILE_ONLY true)" % locals())
		source_groups.append("""\tsource_group("%s\\\\%s" FILES ${%s})""" % (source_group_name, dir_relpath_win32[len(src_dir_name)+1:].replace('\\', '\\\\'), dir_relpath_))		
add_library_files = "\n".join(add_library_files)
globs = "\n".join(globs)
set_source_file_properties = "\n".join(set_source_file_properties)
source_groups = "\n".join(source_groups)
with open(os.path.join(ROOT_DIR_PATH, 'YieldLib.cmake'), 'w+') as f:
	f.write("""\
include(YieldFlags.cmake)	
	
%(globs)s
if (WIN32)
%(set_source_file_properties)s
%(source_groups)s
endif()
add_library(
	yield STATIC
%(add_library_files)s
)
set_target_properties(yield PROPERTIES LINKER_LANGUAGE CXX)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	target_link_libraries(yield pthread)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(yield pthread)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
	target_link_libraries(yield kstat)
endif()

add_subdirectory(share)
""" % locals())
	
