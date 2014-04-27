cmake_minimum_required(VERSION 2.8)

set(TOP_LEVEL_PROJECT_NAME "${PROJECT_NAME}")

project(yield CXX)

if ("${TOP_LEVEL_PROJECT_NAME}" STREQUAL "")
	include(YieldLibs.cmake)
else()
	include(YieldLib.cmake)
endif()
