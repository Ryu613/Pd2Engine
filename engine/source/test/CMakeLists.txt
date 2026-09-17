include(BuildTests)

add_engine_test(engine test_engine_lifecycle.cpp)
#add_engine_test(renderer test_renderer.cpp)
add_engine_test(backend_vulkan test_backend_vulkan.cpp)

target_compile_definitions(test_engine_backend_vulkan PRIVATE ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/")