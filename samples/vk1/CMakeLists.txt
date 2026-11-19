add_executable(vk1 vk1.cpp)

find_package(SDL3 CONFIG REQUIRED)

target_compile_definitions(vk1_lib PUBLIC ENABLE_LOG)
target_link_libraries(vk1 PRIVATE vk1_lib SDL3::SDL3)
target_compile_definitions(vk1 PRIVATE ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/")