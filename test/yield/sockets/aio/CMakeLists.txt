file(GLOB SRC *.cpp *.hpp)
source_group("Source Files" FILES ${SRC})
if (WIN32)
	file(GLOB SRC_WIN32 win32/*.cpp win32/*.hpp)
	source_group("Source Files\\win32" ${SRC_WIN32})
	add_executable(yield.sockets.aio_test ${SRC} ${SRC_WIN32})
else()
	add_executable(yield.sockets.aio_test ${SRC})
endif()
add_test(yield.sockets.aio_test yield.sockets.aio_test --gtest_output=xml:${CMAKE_BINARY_DIR}/gtest_output/yield.socket.aio_test.xml)
target_link_libraries(yield.sockets.aio_test yield.fs yield.sockets.aio gtest)
