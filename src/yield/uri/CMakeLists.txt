file(GLOB CPP *.cpp *.rl ${CMAKE_SOURCE_DIR}/include/yield/uri/*.hpp)
find_package(RAGEL)
if (RAGEL_FOUND)
	RAGEL_TARGET(uri_parser uri_parser.rl uri_parser.cpp)
endif()
add_library(yield.uri STATIC ${CPP})
target_link_libraries(yield.uri yield)
