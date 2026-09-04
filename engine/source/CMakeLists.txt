cmake_minimum_required(VERSION 3.31)

# engine lib
add_library(engine_lib)
# 这句必须加，否则clangd无法识别c++20及以上语法(msvc clang-cl下)
# 原因在于std的latest无法正常识别，clangd的stl会fallback到C++14，导致stl14以上的stl和语法报错
# target_compile_options(engine_lib PUBLIC /std:c++23)
target_compile_features(engine_lib PUBLIC cxx_std_23)

#engine pch
target_precompile_headers(engine_lib PUBLIC include/pd/pch.hpp)

# 3rd-party libs
include(deps/EngineDeps)

target_link_libraries(engine_lib
    PUBLIC
        ${PD2_ENGINE_LIB_PUBLIC_LINKS}
        vendor_lib
    PRIVATE
)

target_sources(engine_lib
    PUBLIC
        # headers
        include/pd/engine.hpp
        # core
        include/pd/core/utils/handle.hpp
        include/pd/core/utils/hash.hpp
        include/pd/core/utils/map.hpp
        include/pd/core/utils/memory.hpp
        include/pd/core/allocators.hpp
        # platform
        include/pd/platform/platform.hpp
        include/pd/platform/fs/file_system.hpp
        include/pd/platform/fs/std_file_system.hpp
        # backend
        include/pd/backend/backend.hpp
        # resource
        include/pd/resource/resource_manager.hpp
        # asset
        include/pd/asset/asset_manager.hpp
        # scene
        include/pd/scene/scene_manager.hpp
        # rendering
        include/pd/rendering/renderer.hpp
    PRIVATE
        src/engine.cpp
        src/platform/fs/file_system.cpp
        src/platform/fs/std_file_system.cpp
        src/platform/platform.cpp
        src/resource/resource_manager.cpp
        src/asset/asset_manager.cpp
        src/scene/scene_manager.cpp
        src/rendering/renderer.cpp
)

# platform
if(WIN32)
 target_sources(engine_lib
    PRIVATE
        src/platform/os/win32/platform_win32.cpp
 )
endif()

if(${PD2_WINDOW} STREQUAL "SDL3")
    target_sources(engine_lib
        PRIVATE
            src/platform/ws/sdl3/window_system_sdl3.cpp
    )
    target_compile_definitions(engine_lib PRIVATE WINDOW_SDL3)
endif()

# backend

if(${PD2_BACKEND} STREQUAL "VULKAN")
target_sources(engine_lib
    PRIVATE
        src/backend/vulkan/backend_vulkan.cpp
)
target_compile_definitions(engine_lib PRIVATE BACKEND_VULKAN)
endif()


target_include_directories(engine_lib
    PUBLIC
        include
        ${PD2_ENGINE_LIB_PUBLIC_INCLUDE}
        # export macro header file
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    PRIVATE
        src
)

generate_export_header(engine_lib BASE_NAME pd2_engine)
configure_file(
    ${CMAKE_CURRENT_BINARY_DIR}/pd2_engine_export.h ${CMAKE_BINARY_DIR}/include/pd/pd2_engine_export.h
)

if(PD2_BUILD_TESTS)
    add_subdirectory(test)
endif()
