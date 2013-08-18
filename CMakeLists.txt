cmake_minimum_required(VERSION 2.8)

project(yield CXX)

if (YIELD_FUSED)
	include(YieldLib.cmake)
else()
	include(YieldLibs.cmake)
endif()
