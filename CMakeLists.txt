cmake_minimum_required(VERSION 3.31)

include(cmake/base.cmake)

project(PD2
    VERSION 0.1.0
    DESCRIPTION "Pd2Engine: A modern lightweight realtime graphics engine"
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(PD2_BUILD_TESTS "include test suit and build unit test code" ON)
option(PD2_BUILD_SAMPLES "build engine samples(in directory: samples/)" OFF)
option(PD2_BUILD_LIBS "build libs with using engine or used by engine" OFF)

set(PD2_BACKEND "VULKAN")
set(PD2_WSI "SDL3")

message(STATUS "BUILD_TESTS:${PD2_BUILD_TESTS}")
message(STATUS "BUILD_SAMPLES:${PD2_BUILD_SAMPLES}")
message(STATUS "BUILD_LIBS:${PD2_BUILD_LIBS}")
message(STATUS "USE_BACKEND:${PD2_BACKEND}")
message(STATUS "USE_WSI:${PD2_WSI}")

# engine
add_subdirectory(engine/source)

if(BUILD_SAMPLES)
  add_subdirectory(samples)
endif()

if(BUILD_LIBS)
  add_subdirectory(libs)
endif()