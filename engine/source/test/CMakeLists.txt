cmake_minimum_required(VERSION 3.31)
include(${PD2_ROOT_DIR}/cmake/build_tests.cmake)

add_executable(test_engine_lifecycle test_engine_lifecycle.cpp)
target_link_libraries(test_engine_lifecycle PRIVATE engine_lib Catch2::Catch2WithMain)

add_executable(test_arena test_arena.cpp)
target_link_libraries(test_arena PRIVATE engine_lib Catch2::Catch2WithMain)

# add_executable(test_scene test_scene.cpp)
# target_link_libraries(test_scene PRIVATE engine_lib Catch2::Catch2WithMain)

add_executable(test_asset test_asset.cpp)
target_link_libraries(test_asset PRIVATE engine_lib Catch2::Catch2WithMain)