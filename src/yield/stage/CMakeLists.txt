file(GLOB CPP *.cpp ${CMAKE_SOURCE_DIR}/include/yield/stage/*.hpp)
add_library(yield.stage STATIC ${CPP})
target_link_libraries(yield.stage yield.thread)
