add_library(sandbox_lib STATIC)
target_sources(sandbox_lib
    PUBLIC
    world.hpp
    PRIVATE
    world.cpp

)
target_link_libraries(sandbox_lib PUBLIC engine_lib)

target_compile_definitions(sandbox_lib PRIVATE ASSET_BASE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
