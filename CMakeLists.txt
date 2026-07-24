cmake_minimum_required(VERSION 3.31)

include(cmake/ProjectBase.cmake)

option(PD2_BUILD_TESTS "build unit test" ON)
option(PD2_BUILD_SAMPLES "build engine samples" OFF)
option(PD2_BUILD_LIBS "build libs" OFF)
option(PD2_BUILD_DOCS "Build documentation" OFF)

set(PD2_BACKEND "VULKAN")
set(PD2_WINDOW "SDL3")
set(PD2_SHADER_LANG "SLANG")

message(STATUS "BUILD_TESTS:${PD2_BUILD_TESTS}")
message(STATUS "BUILD_SAMPLES:${PD2_BUILD_SAMPLES}")
message(STATUS "BUILD_LIBS:${PD2_BUILD_LIBS}")
message(STATUS "USE_BACKEND:${PD2_BACKEND}")
message(STATUS "USE_WINDOW:${PD2_WINDOW}")
message(STATUS "USE_SHADER_LANG:${PD2_SHADER_LANG}")

# currently only support vulkan, slangc, sdl3
if(PD2_BACKEND STREQUAL "VULKAN")
    list(APPEND VCPKG_MANIFEST_FEATURES "backend-vulkan")
else()
    message(FATAL_ERROR "USE_BACKEND not correct: ${PD2_BACKEND}")
endif()

if(PD2_SHADER_LANG STREQUAL "SLANG")
    list(APPEND VCPKG_MANIFEST_FEATURES "slang")
else()
    message(FATAL_ERROR "PD2_SHADER_LANG not correct: ${PD2_SHADER_LANG}")
endif()

project(PD2
    VERSION 0.1.0
    DESCRIPTION "Pd2Engine: A modern C++ 3D realtime rendering engine"
)

include(GenerateExportHeader)
include(CompileShaders)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# put 3rd party libs in the same dir, must be set before including libs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# engine
add_subdirectory(engine)

if(PD2_BUILD_SAMPLES)
  add_subdirectory(samples)
endif()

if(PD2_BUILD_LIBS)
  add_subdirectory(libs)
endif()

if(PD2_BUILD_DOCS)
  add_subdirectory(docs)
endif()