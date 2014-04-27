include(YieldFlags.cmake)	
	
file(GLOB INCLUDE_ "include/*.h" "include/*.hpp" "include/*.cpp" "include/*.h" "include/*.rl")
file(GLOB INCLUDE_YIELD "include/yield/*.h" "include/yield/*.hpp" "include/yield/*.cpp" "include/yield/*.h" "include/yield/*.rl")
file(GLOB INCLUDE_YIELD_FS "include/yield/fs/*.h" "include/yield/fs/*.hpp" "include/yield/fs/*.cpp" "include/yield/fs/*.h" "include/yield/fs/*.rl")
file(GLOB INCLUDE_YIELD_FS_POLL "include/yield/fs/poll/*.h" "include/yield/fs/poll/*.hpp" "include/yield/fs/poll/*.cpp" "include/yield/fs/poll/*.h" "include/yield/fs/poll/*.rl")
file(GLOB INCLUDE_YIELD_HTTP "include/yield/http/*.h" "include/yield/http/*.hpp" "include/yield/http/*.cpp" "include/yield/http/*.h" "include/yield/http/*.rl")
file(GLOB INCLUDE_YIELD_HTTP_SERVER "include/yield/http/server/*.h" "include/yield/http/server/*.hpp" "include/yield/http/server/*.cpp" "include/yield/http/server/*.h" "include/yield/http/server/*.rl")
file(GLOB INCLUDE_YIELD_HTTP_SERVER_FILE "include/yield/http/server/file/*.h" "include/yield/http/server/file/*.hpp" "include/yield/http/server/file/*.cpp" "include/yield/http/server/file/*.h" "include/yield/http/server/file/*.rl")
file(GLOB INCLUDE_YIELD_HTTP_SERVER_YGI "include/yield/http/server/ygi/*.h" "include/yield/http/server/ygi/*.hpp" "include/yield/http/server/ygi/*.cpp" "include/yield/http/server/ygi/*.h" "include/yield/http/server/ygi/*.rl")
file(GLOB INCLUDE_YIELD_I18N "include/yield/i18n/*.h" "include/yield/i18n/*.hpp" "include/yield/i18n/*.cpp" "include/yield/i18n/*.h" "include/yield/i18n/*.rl")
file(GLOB INCLUDE_YIELD_POLL "include/yield/poll/*.h" "include/yield/poll/*.hpp" "include/yield/poll/*.cpp" "include/yield/poll/*.h" "include/yield/poll/*.rl")
file(GLOB INCLUDE_YIELD_QUEUE "include/yield/queue/*.h" "include/yield/queue/*.hpp" "include/yield/queue/*.cpp" "include/yield/queue/*.h" "include/yield/queue/*.rl")
file(GLOB INCLUDE_YIELD_SOCKETS "include/yield/sockets/*.h" "include/yield/sockets/*.hpp" "include/yield/sockets/*.cpp" "include/yield/sockets/*.h" "include/yield/sockets/*.rl")
file(GLOB INCLUDE_YIELD_SOCKETS_AIO "include/yield/sockets/aio/*.h" "include/yield/sockets/aio/*.hpp" "include/yield/sockets/aio/*.cpp" "include/yield/sockets/aio/*.h" "include/yield/sockets/aio/*.rl")
file(GLOB INCLUDE_YIELD_SOCKETS_SSL "include/yield/sockets/ssl/*.h" "include/yield/sockets/ssl/*.hpp" "include/yield/sockets/ssl/*.cpp" "include/yield/sockets/ssl/*.h" "include/yield/sockets/ssl/*.rl")
file(GLOB INCLUDE_YIELD_STAGE "include/yield/stage/*.h" "include/yield/stage/*.hpp" "include/yield/stage/*.cpp" "include/yield/stage/*.h" "include/yield/stage/*.rl")
file(GLOB INCLUDE_YIELD_THREAD "include/yield/thread/*.h" "include/yield/thread/*.hpp" "include/yield/thread/*.cpp" "include/yield/thread/*.h" "include/yield/thread/*.rl")
file(GLOB INCLUDE_YIELD_URI "include/yield/uri/*.h" "include/yield/uri/*.hpp" "include/yield/uri/*.cpp" "include/yield/uri/*.h" "include/yield/uri/*.rl")
file(GLOB SRC_ "src/*.h" "src/*.hpp" "src/*.cpp" "src/*.h" "src/*.rl")
file(GLOB SRC_YIELD "src/yield/*.h" "src/yield/*.hpp" "src/yield/*.cpp" "src/yield/*.h" "src/yield/*.rl")
file(GLOB SRC_YIELD_FS "src/yield/fs/*.h" "src/yield/fs/*.hpp" "src/yield/fs/*.cpp" "src/yield/fs/*.h" "src/yield/fs/*.rl")
file(GLOB SRC_YIELD_FS_POLL "src/yield/fs/poll/*.h" "src/yield/fs/poll/*.hpp" "src/yield/fs/poll/*.cpp" "src/yield/fs/poll/*.h" "src/yield/fs/poll/*.rl")
file(GLOB SRC_YIELD_FS_POLL_BSD "src/yield/fs/poll/bsd/*.h" "src/yield/fs/poll/bsd/*.hpp" "src/yield/fs/poll/bsd/*.cpp" "src/yield/fs/poll/bsd/*.h" "src/yield/fs/poll/bsd/*.rl")
file(GLOB SRC_YIELD_FS_POLL_LINUX "src/yield/fs/poll/linux/*.h" "src/yield/fs/poll/linux/*.hpp" "src/yield/fs/poll/linux/*.cpp" "src/yield/fs/poll/linux/*.h" "src/yield/fs/poll/linux/*.rl")
file(GLOB SRC_YIELD_FS_POLL_WIN32 "src/yield/fs/poll/win32/*.h" "src/yield/fs/poll/win32/*.hpp" "src/yield/fs/poll/win32/*.cpp" "src/yield/fs/poll/win32/*.h" "src/yield/fs/poll/win32/*.rl")
file(GLOB SRC_YIELD_FS_POSIX "src/yield/fs/posix/*.h" "src/yield/fs/posix/*.hpp" "src/yield/fs/posix/*.cpp" "src/yield/fs/posix/*.h" "src/yield/fs/posix/*.rl")
file(GLOB SRC_YIELD_FS_WIN32 "src/yield/fs/win32/*.h" "src/yield/fs/win32/*.hpp" "src/yield/fs/win32/*.cpp" "src/yield/fs/win32/*.h" "src/yield/fs/win32/*.rl")
file(GLOB SRC_YIELD_HTTP "src/yield/http/*.h" "src/yield/http/*.hpp" "src/yield/http/*.cpp" "src/yield/http/*.h" "src/yield/http/*.rl")
file(GLOB SRC_YIELD_HTTP_SERVER "src/yield/http/server/*.h" "src/yield/http/server/*.hpp" "src/yield/http/server/*.cpp" "src/yield/http/server/*.h" "src/yield/http/server/*.rl")
file(GLOB SRC_YIELD_HTTP_SERVER_FILE "src/yield/http/server/file/*.h" "src/yield/http/server/file/*.hpp" "src/yield/http/server/file/*.cpp" "src/yield/http/server/file/*.h" "src/yield/http/server/file/*.rl")
file(GLOB SRC_YIELD_HTTP_SERVER_YGI "src/yield/http/server/ygi/*.h" "src/yield/http/server/ygi/*.hpp" "src/yield/http/server/ygi/*.cpp" "src/yield/http/server/ygi/*.h" "src/yield/http/server/ygi/*.rl")
file(GLOB SRC_YIELD_I18N "src/yield/i18n/*.h" "src/yield/i18n/*.hpp" "src/yield/i18n/*.cpp" "src/yield/i18n/*.h" "src/yield/i18n/*.rl")
file(GLOB SRC_YIELD_I18N_POSIX "src/yield/i18n/posix/*.h" "src/yield/i18n/posix/*.hpp" "src/yield/i18n/posix/*.cpp" "src/yield/i18n/posix/*.h" "src/yield/i18n/posix/*.rl")
file(GLOB SRC_YIELD_I18N_WIN32 "src/yield/i18n/win32/*.h" "src/yield/i18n/win32/*.hpp" "src/yield/i18n/win32/*.cpp" "src/yield/i18n/win32/*.h" "src/yield/i18n/win32/*.rl")
file(GLOB SRC_YIELD_POLL "src/yield/poll/*.h" "src/yield/poll/*.hpp" "src/yield/poll/*.cpp" "src/yield/poll/*.h" "src/yield/poll/*.rl")
file(GLOB SRC_YIELD_POLL_BSD "src/yield/poll/bsd/*.h" "src/yield/poll/bsd/*.hpp" "src/yield/poll/bsd/*.cpp" "src/yield/poll/bsd/*.h" "src/yield/poll/bsd/*.rl")
file(GLOB SRC_YIELD_POLL_LINUX "src/yield/poll/linux/*.h" "src/yield/poll/linux/*.hpp" "src/yield/poll/linux/*.cpp" "src/yield/poll/linux/*.h" "src/yield/poll/linux/*.rl")
file(GLOB SRC_YIELD_POLL_POSIX "src/yield/poll/posix/*.h" "src/yield/poll/posix/*.hpp" "src/yield/poll/posix/*.cpp" "src/yield/poll/posix/*.h" "src/yield/poll/posix/*.rl")
file(GLOB SRC_YIELD_POLL_SUNOS "src/yield/poll/sunos/*.h" "src/yield/poll/sunos/*.hpp" "src/yield/poll/sunos/*.cpp" "src/yield/poll/sunos/*.h" "src/yield/poll/sunos/*.rl")
file(GLOB SRC_YIELD_POLL_WIN32 "src/yield/poll/win32/*.h" "src/yield/poll/win32/*.hpp" "src/yield/poll/win32/*.cpp" "src/yield/poll/win32/*.h" "src/yield/poll/win32/*.rl")
file(GLOB SRC_YIELD_QUEUE "src/yield/queue/*.h" "src/yield/queue/*.hpp" "src/yield/queue/*.cpp" "src/yield/queue/*.h" "src/yield/queue/*.rl")
file(GLOB SRC_YIELD_SOCKETS "src/yield/sockets/*.h" "src/yield/sockets/*.hpp" "src/yield/sockets/*.cpp" "src/yield/sockets/*.h" "src/yield/sockets/*.rl")
file(GLOB SRC_YIELD_SOCKETS_AIO "src/yield/sockets/aio/*.h" "src/yield/sockets/aio/*.hpp" "src/yield/sockets/aio/*.cpp" "src/yield/sockets/aio/*.h" "src/yield/sockets/aio/*.rl")
file(GLOB SRC_YIELD_SOCKETS_AIO_WIN32 "src/yield/sockets/aio/win32/*.h" "src/yield/sockets/aio/win32/*.hpp" "src/yield/sockets/aio/win32/*.cpp" "src/yield/sockets/aio/win32/*.h" "src/yield/sockets/aio/win32/*.rl")
file(GLOB SRC_YIELD_SOCKETS_POSIX "src/yield/sockets/posix/*.h" "src/yield/sockets/posix/*.hpp" "src/yield/sockets/posix/*.cpp" "src/yield/sockets/posix/*.h" "src/yield/sockets/posix/*.rl")
file(GLOB SRC_YIELD_SOCKETS_SSL "src/yield/sockets/ssl/*.h" "src/yield/sockets/ssl/*.hpp" "src/yield/sockets/ssl/*.cpp" "src/yield/sockets/ssl/*.h" "src/yield/sockets/ssl/*.rl")
file(GLOB SRC_YIELD_SOCKETS_WIN32 "src/yield/sockets/win32/*.h" "src/yield/sockets/win32/*.hpp" "src/yield/sockets/win32/*.cpp" "src/yield/sockets/win32/*.h" "src/yield/sockets/win32/*.rl")
file(GLOB SRC_YIELD_STAGE "src/yield/stage/*.h" "src/yield/stage/*.hpp" "src/yield/stage/*.cpp" "src/yield/stage/*.h" "src/yield/stage/*.rl")
file(GLOB SRC_YIELD_THREAD "src/yield/thread/*.h" "src/yield/thread/*.hpp" "src/yield/thread/*.cpp" "src/yield/thread/*.h" "src/yield/thread/*.rl")
file(GLOB SRC_YIELD_THREAD_DARWIN "src/yield/thread/darwin/*.h" "src/yield/thread/darwin/*.hpp" "src/yield/thread/darwin/*.cpp" "src/yield/thread/darwin/*.h" "src/yield/thread/darwin/*.rl")
file(GLOB SRC_YIELD_THREAD_LINUX "src/yield/thread/linux/*.h" "src/yield/thread/linux/*.hpp" "src/yield/thread/linux/*.cpp" "src/yield/thread/linux/*.h" "src/yield/thread/linux/*.rl")
file(GLOB SRC_YIELD_THREAD_POSIX "src/yield/thread/posix/*.h" "src/yield/thread/posix/*.hpp" "src/yield/thread/posix/*.cpp" "src/yield/thread/posix/*.h" "src/yield/thread/posix/*.rl")
file(GLOB SRC_YIELD_THREAD_SUNOS "src/yield/thread/sunos/*.h" "src/yield/thread/sunos/*.hpp" "src/yield/thread/sunos/*.cpp" "src/yield/thread/sunos/*.h" "src/yield/thread/sunos/*.rl")
file(GLOB SRC_YIELD_THREAD_WIN32 "src/yield/thread/win32/*.h" "src/yield/thread/win32/*.hpp" "src/yield/thread/win32/*.cpp" "src/yield/thread/win32/*.h" "src/yield/thread/win32/*.rl")
file(GLOB SRC_YIELD_URI "src/yield/uri/*.h" "src/yield/uri/*.hpp" "src/yield/uri/*.cpp" "src/yield/uri/*.h" "src/yield/uri/*.rl")
if (WIN32)
	set_source_files_properties(${SRC_YIELD_FS_POLL_BSD} PROPERTIES HEADER_FILE_ONLY true)
	set_source_files_properties(${SRC_YIELD_FS_POLL_LINUX} PROPERTIES HEADER_FILE_ONLY true)
	set_source_files_properties(${SRC_YIELD_FS_POSIX} PROPERTIES HEADER_FILE_ONLY true)
	set_source_files_properties(${SRC_YIELD_I18N_POSIX} PROPERTIES HEADER_FILE_ONLY true)
	set_source_files_properties(${SRC_YIELD_POLL_BSD} PROPERTIES HEADER_FILE_ONLY true)
	set_source_files_properties(${SRC_YIELD_POLL_LINUX} PROPERTIES HEADER_FILE_ONLY true)
	set_source_files_properties(${SRC_YIELD_POLL_POSIX} PROPERTIES HEADER_FILE_ONLY true)
	set_source_files_properties(${SRC_YIELD_POLL_SUNOS} PROPERTIES HEADER_FILE_ONLY true)
	set_source_files_properties(${SRC_YIELD_SOCKETS_POSIX} PROPERTIES HEADER_FILE_ONLY true)
	set_source_files_properties(${SRC_YIELD_THREAD_DARWIN} PROPERTIES HEADER_FILE_ONLY true)
	set_source_files_properties(${SRC_YIELD_THREAD_LINUX} PROPERTIES HEADER_FILE_ONLY true)
	set_source_files_properties(${SRC_YIELD_THREAD_POSIX} PROPERTIES HEADER_FILE_ONLY true)
	set_source_files_properties(${SRC_YIELD_THREAD_SUNOS} PROPERTIES HEADER_FILE_ONLY true)
	source_group("Header Files\\" FILES ${INCLUDE_})
	source_group("Header Files\\yield" FILES ${INCLUDE_YIELD})
	source_group("Header Files\\yield\\fs" FILES ${INCLUDE_YIELD_FS})
	source_group("Header Files\\yield\\fs\\poll" FILES ${INCLUDE_YIELD_FS_POLL})
	source_group("Header Files\\yield\\http" FILES ${INCLUDE_YIELD_HTTP})
	source_group("Header Files\\yield\\http\\server" FILES ${INCLUDE_YIELD_HTTP_SERVER})
	source_group("Header Files\\yield\\http\\server\\file" FILES ${INCLUDE_YIELD_HTTP_SERVER_FILE})
	source_group("Header Files\\yield\\http\\server\\ygi" FILES ${INCLUDE_YIELD_HTTP_SERVER_YGI})
	source_group("Header Files\\yield\\i18n" FILES ${INCLUDE_YIELD_I18N})
	source_group("Header Files\\yield\\poll" FILES ${INCLUDE_YIELD_POLL})
	source_group("Header Files\\yield\\queue" FILES ${INCLUDE_YIELD_QUEUE})
	source_group("Header Files\\yield\\sockets" FILES ${INCLUDE_YIELD_SOCKETS})
	source_group("Header Files\\yield\\sockets\\aio" FILES ${INCLUDE_YIELD_SOCKETS_AIO})
	source_group("Header Files\\yield\\sockets\\ssl" FILES ${INCLUDE_YIELD_SOCKETS_SSL})
	source_group("Header Files\\yield\\stage" FILES ${INCLUDE_YIELD_STAGE})
	source_group("Header Files\\yield\\thread" FILES ${INCLUDE_YIELD_THREAD})
	source_group("Header Files\\yield\\uri" FILES ${INCLUDE_YIELD_URI})
	source_group("Source Files\\" FILES ${SRC_})
	source_group("Source Files\\yield" FILES ${SRC_YIELD})
	source_group("Source Files\\yield\\fs" FILES ${SRC_YIELD_FS})
	source_group("Source Files\\yield\\fs\\poll" FILES ${SRC_YIELD_FS_POLL})
	source_group("Source Files\\yield\\fs\\poll\\bsd" FILES ${SRC_YIELD_FS_POLL_BSD})
	source_group("Source Files\\yield\\fs\\poll\\linux" FILES ${SRC_YIELD_FS_POLL_LINUX})
	source_group("Source Files\\yield\\fs\\poll\\win32" FILES ${SRC_YIELD_FS_POLL_WIN32})
	source_group("Source Files\\yield\\fs\\posix" FILES ${SRC_YIELD_FS_POSIX})
	source_group("Source Files\\yield\\fs\\win32" FILES ${SRC_YIELD_FS_WIN32})
	source_group("Source Files\\yield\\http" FILES ${SRC_YIELD_HTTP})
	source_group("Source Files\\yield\\http\\server" FILES ${SRC_YIELD_HTTP_SERVER})
	source_group("Source Files\\yield\\http\\server\\file" FILES ${SRC_YIELD_HTTP_SERVER_FILE})
	source_group("Source Files\\yield\\http\\server\\ygi" FILES ${SRC_YIELD_HTTP_SERVER_YGI})
	source_group("Source Files\\yield\\i18n" FILES ${SRC_YIELD_I18N})
	source_group("Source Files\\yield\\i18n\\posix" FILES ${SRC_YIELD_I18N_POSIX})
	source_group("Source Files\\yield\\i18n\\win32" FILES ${SRC_YIELD_I18N_WIN32})
	source_group("Source Files\\yield\\poll" FILES ${SRC_YIELD_POLL})
	source_group("Source Files\\yield\\poll\\bsd" FILES ${SRC_YIELD_POLL_BSD})
	source_group("Source Files\\yield\\poll\\linux" FILES ${SRC_YIELD_POLL_LINUX})
	source_group("Source Files\\yield\\poll\\posix" FILES ${SRC_YIELD_POLL_POSIX})
	source_group("Source Files\\yield\\poll\\sunos" FILES ${SRC_YIELD_POLL_SUNOS})
	source_group("Source Files\\yield\\poll\\win32" FILES ${SRC_YIELD_POLL_WIN32})
	source_group("Source Files\\yield\\queue" FILES ${SRC_YIELD_QUEUE})
	source_group("Source Files\\yield\\sockets" FILES ${SRC_YIELD_SOCKETS})
	source_group("Source Files\\yield\\sockets\\aio" FILES ${SRC_YIELD_SOCKETS_AIO})
	source_group("Source Files\\yield\\sockets\\aio\\win32" FILES ${SRC_YIELD_SOCKETS_AIO_WIN32})
	source_group("Source Files\\yield\\sockets\\posix" FILES ${SRC_YIELD_SOCKETS_POSIX})
	source_group("Source Files\\yield\\sockets\\ssl" FILES ${SRC_YIELD_SOCKETS_SSL})
	source_group("Source Files\\yield\\sockets\\win32" FILES ${SRC_YIELD_SOCKETS_WIN32})
	source_group("Source Files\\yield\\stage" FILES ${SRC_YIELD_STAGE})
	source_group("Source Files\\yield\\thread" FILES ${SRC_YIELD_THREAD})
	source_group("Source Files\\yield\\thread\\darwin" FILES ${SRC_YIELD_THREAD_DARWIN})
	source_group("Source Files\\yield\\thread\\linux" FILES ${SRC_YIELD_THREAD_LINUX})
	source_group("Source Files\\yield\\thread\\posix" FILES ${SRC_YIELD_THREAD_POSIX})
	source_group("Source Files\\yield\\thread\\sunos" FILES ${SRC_YIELD_THREAD_SUNOS})
	source_group("Source Files\\yield\\thread\\win32" FILES ${SRC_YIELD_THREAD_WIN32})
	source_group("Source Files\\yield\\uri" FILES ${SRC_YIELD_URI})
endif()
add_library(
	yield STATIC
	${INCLUDE_}
	${INCLUDE_YIELD}
	${INCLUDE_YIELD_FS}
	${INCLUDE_YIELD_FS_POLL}
	${INCLUDE_YIELD_HTTP}
	${INCLUDE_YIELD_HTTP_SERVER}
	${INCLUDE_YIELD_HTTP_SERVER_FILE}
	${INCLUDE_YIELD_HTTP_SERVER_YGI}
	${INCLUDE_YIELD_I18N}
	${INCLUDE_YIELD_POLL}
	${INCLUDE_YIELD_QUEUE}
	${INCLUDE_YIELD_SOCKETS}
	${INCLUDE_YIELD_SOCKETS_AIO}
	${INCLUDE_YIELD_SOCKETS_SSL}
	${INCLUDE_YIELD_STAGE}
	${INCLUDE_YIELD_THREAD}
	${INCLUDE_YIELD_URI}
	${SRC_}
	${SRC_YIELD}
	${SRC_YIELD_FS}
	${SRC_YIELD_FS_POLL}
	${SRC_YIELD_FS_POLL_BSD}
	${SRC_YIELD_FS_POLL_LINUX}
	${SRC_YIELD_FS_POLL_WIN32}
	${SRC_YIELD_FS_POSIX}
	${SRC_YIELD_FS_WIN32}
	${SRC_YIELD_HTTP}
	${SRC_YIELD_HTTP_SERVER}
	${SRC_YIELD_HTTP_SERVER_FILE}
	${SRC_YIELD_HTTP_SERVER_YGI}
	${SRC_YIELD_I18N}
	${SRC_YIELD_I18N_POSIX}
	${SRC_YIELD_I18N_WIN32}
	${SRC_YIELD_POLL}
	${SRC_YIELD_POLL_BSD}
	${SRC_YIELD_POLL_LINUX}
	${SRC_YIELD_POLL_POSIX}
	${SRC_YIELD_POLL_SUNOS}
	${SRC_YIELD_POLL_WIN32}
	${SRC_YIELD_QUEUE}
	${SRC_YIELD_SOCKETS}
	${SRC_YIELD_SOCKETS_AIO}
	${SRC_YIELD_SOCKETS_AIO_WIN32}
	${SRC_YIELD_SOCKETS_POSIX}
	${SRC_YIELD_SOCKETS_SSL}
	${SRC_YIELD_SOCKETS_WIN32}
	${SRC_YIELD_STAGE}
	${SRC_YIELD_THREAD}
	${SRC_YIELD_THREAD_DARWIN}
	${SRC_YIELD_THREAD_LINUX}
	${SRC_YIELD_THREAD_POSIX}
	${SRC_YIELD_THREAD_SUNOS}
	${SRC_YIELD_THREAD_WIN32}
	${SRC_YIELD_URI}
)
set_target_properties(yield PROPERTIES LINKER_LANGUAGE CXX)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	target_link_libraries(yield pthread)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(yield pthread)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
	target_link_libraries(yield kstat)
endif()