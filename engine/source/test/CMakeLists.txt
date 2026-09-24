include(BuildTests)

add_engine_test(engine test_engine_lifecycle.cpp)
#add_engine_test(renderer test_renderer.cpp)
add_engine_test(backend_vulkan test_backend_vulkan.cpp)
add_engine_test(asset_mgr test_asset_mgr.cpp)
add_engine_test(resource_mgr test_resource_mgr.cpp)

