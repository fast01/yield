file(GLOB SRC *.cpp *.hpp)
source_group("Source Files" FILES ${SRC})
add_executable(yield.http_test ${SRC})
add_test(yield.http_test yield.http_test --gtest_output=xml:${CMAKE_BINARY_DIR}/gtest_output/yield.http_test.xml)
target_link_libraries(yield.http_test yield.http gtest)

add_subdirectory(server)
