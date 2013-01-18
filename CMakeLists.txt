cmake_minimum_required(VERSION 2.8)

project(yield CXX)

# Definitions
add_definitions(-D__STDC_LIMIT_MACROS=1)
if (MSVC)
	if (CMAKE_GENERATOR MATCHES "Visual Studio 11") # VS2012 doesn't support correctly the tuples yet
		add_definitions(/D _VARIADIC_MAX=10)
	endif()
	add_definitions(-D_CRT_DISABLE_PERFCRIT_LOCKS=1) # CRT options
	add_definitions(-DSTRICT=1 -DUNICODE=1 -D_WIN32_WINNT=0x0600 -DWIN32_LEAN_AND_MEAN) # windows.h options
else()
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_definitions(-D_DEBUG)
	endif()
	add_definitions(-D_FILE_OFFSET_BITS=64)
	if (CMAKE_SYSTEM_NAME MATCHES "SunOS")
		add_definitions(-D_REENTRANT -Dupgrade_the_compiler_to_use_STL=1)
	endif()
endif()

# CXX_FLAGS
if (MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /GR-")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")	
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MP")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od")
	# /Zi = Program Database; /ZI Program Database for Edit and Continue is not available on Win64
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
	# Crank up warnings
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Wall")
	# Disable specific warnings
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4021") # nonstandard extension used : nameless struct/union
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4061") # enumerator 'identifier' in switch of enum 'enumeration' is not explicitly handled by a case label
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4100") # unused formal parameter
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4181") # qualifier applied to reference type ignored	
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4263") # 'function' : member function does not override any base class virtual member function
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4264") # 'virtual_function' : no override available for virtual member function from base 'class'; function is hidden
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4266") # 'function' : no override available for virtual member function from base 'type'; function is hidden
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4290") # exception specification
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4350") # behavior change: 'member1' called instead of 'member2'
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4371") # layout of class may have changed from a previous version of the compiler due to better packing of member
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4512") # 'ClassName' : assignment operator could not be generated
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4514") # 'FuncName' : unreferenced inline function has been removed
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4548") # expression before comma has no effect; expected expression with side-effect
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4571") # Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4625") # 'derived class' : copy constructor could not be generated because a base class copy constructor is inaccessible
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4626") # 'derived class' : assignment operator could not be generated because a base class assignment operator is inaccessible
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4668") # 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4710") # function 'FuncName' not expanded
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4711") # function 'function' selected for inline expansion
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4786") # identifier was truncated to 'number' characters in the debug information
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4820") # 'bytes' bytes padding added after construct 'member_name'
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4987") # nonstandard extension used: 'throw (...)'
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4996") # 'function': was declared deprecated (triggered by POSIX functions that should be _-prefixed)
	
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GR-")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MP")
else()
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-rtti")
	# Crank up warnings
	# Do not use -Wextra, because it turns on -Wunused-parameter.
        # Instead use some of the warnings implied by -Wextra.
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wfatal-errors")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wfloat-equal")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Winit-self")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Winline")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-multichar")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wsign-compare")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wtype-limits")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wuninitialized")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wunused")
	
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -fno-rtti")	
endif()

# LINKER_FLAGS
if (WIN32)
	set(LINKER_FLAGS_DEBUG "/DEBUG")
	set(LINKER_FLAGS_DEBUG "${LINKER_FLAGS_DEBUG} /ignore:4099")
	set(LINKER_FLAGS_DEBUG "${LINKER_FLAGS_DEBUG} /ignore:4221")
	set(LINKER_FLAGS_DEBUG "${LINKER_FLAGS_DEBUG} /NODEFAULTLIB:libcmt.lib")
	set(LINKER_FLAGS_DEBUG "${LINKER_FLAGS_DEBUG} /NODEFAULTLIB:msvcrt.lib")
	set(LINKER_FLAGS_DEBUG "${LINKER_FLAGS_DEBUG} /NODEFAULTLIB:libcmtd.lib")
	set(LINKER_FLAGS_DEBUG "${LINKER_FLAGS_DEBUG} /NODEFAULTLIB:msvcr71d.lib")
	set(LINKER_FLAGS_DEBUG "${LINKER_FLAGS_DEBUG} /NODEFAULTLIB:msvcr80.lib")
	set(LINKER_FLAGS_DEBUG "${LINKER_FLAGS_DEBUG} /NODEFAULTLIB:msvcr90.lib")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG ${LINKER_FLAGS_DEBUG})
	set(CMAKE_SHARED_LINKER_FLAGS_DEBUG ${LINKER_FLAGS_DEBUG})

	set(LINKER_FLAGS_RELEASE "${LINKER_FLAGS_RELEASE} /NODEFAULTLIB:libcmtd.lib")
	set(LINKER_FLAGS_RELEASE "${LINKER_FLAGS_RELEASE} /NODEFAULTLIB:msvcrtd.lib")
	set(LINKER_FLAGS_RELEASE "${LINKER_FLAGS_RELEASE} /NODEFAULTLIB:libcmtd.lib")
	set(LINKER_FLAGS_RELEASE "${LINKER_FLAGS_RELEASE} /NODEFAULTLIB:msvcr71d.lib")
	set(LINKER_FLAGS_RELEASE "${LINKER_FLAGS_RELEASE} /NODEFAULTLIB:msvcr80d.lib")
	set(LINKER_FLAGS_RELEASE "${LINKER_FLAGS_RELEASE} /NODEFAULTLIB:msvcr90d.lib")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE ${LINKER_FLAGS_RELEASE})
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE ${LINKER_FLAGS_RELEASE})
elseif (YIELD_PLATFORM_NAME STREQUAL "darwin")
	set(CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS "${CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS} -undefined dynamic_lookup")
endif()

enable_testing()

include_directories(
	include
	share/gtest-1.6.0/fused-src
)

set(CMAKE_USE_RELATIVE_PATHS TRUE FORCE)

add_subdirectory(share)
add_subdirectory(src)
add_subdirectory(test)
