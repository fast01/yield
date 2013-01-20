file(GLOB SRC *.cpp *.hpp)
source_group("Source Files" FILES ${SRC})
add_executable(yield.sockets_test ${SRC})
add_test(yield.sockets_test yield.sockets_test --gtest_output=xml:${CMAKE_BINARY_DIR}/gtest_output/yield.sockets_test.xml)
target_link_libraries(yield.sockets_test yield.fs yield.sockets gtest)

add_subdirectory(aio)
