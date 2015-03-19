file(GLOB INCLUDE_ ${CMAKE_SOURCE_DIR}/include/yield/http/server/*.hpp)
file(GLOB SRC *.cpp *.hpp)
source_group("Header Files\\" FILES ${INCLUDE_})
source_group("Source Files\\" FILES ${SRC})
add_library(yield.http.server STATIC ${INCLUDE_} ${SRC})
target_link_libraries(yield.http.server yield.fs yield.http yield.sockets.aio yield.stage)

add_subdirectory(ygi)
