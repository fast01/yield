file(GLOB SRC *.cpp *.hpp)
source_group("Source Files" FILES ${SRC})
add_executable(yield.fs_test ${SRC})
add_test(yield.fs_test yield.fs_test --gtest_output=xml:${CMAKE_BINARY_DIR}/gtest_output/yield.fs_test.xml)
target_link_libraries(yield.fs_test yield.fs gtest)

add_subdirectory(poll)
