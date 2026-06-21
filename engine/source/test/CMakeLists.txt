cmake_minimum_required(VERSION 3.31)
include(BuildTests)

add_engine_test(lifecycle test_engine_lifecycle.cpp)
add_engine_test(backend test_backend.cpp)
add_engine_test(resource_manager test_resource_manager.cpp)
add_engine_test(asset test_asset.cpp)
#add_engine_test(scene test_scene.cpp)