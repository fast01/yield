enable_testing()

include(YieldFlags.cmake)

include_directories(
	include
	share/gtest-1.6.0/fused-src
)

add_subdirectory(share)
add_subdirectory(src)
add_subdirectory(test)
