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
# vendor lib
# include(deps/VendorDeps)

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
    include/pd/core/globals.hpp
    include/pd/core/logging.hpp
    include/pd/core/assert.hpp
    include/pd/core/utils/handle.hpp
    include/pd/core/utils/memory.hpp
    include/pd/core/allocators.hpp
    include/pd/core/context.hpp
    # platform
    include/pd/platform/platform.hpp
    include/pd/platform/platform_factory.hpp
    include/pd/platform/window/window.hpp
    #platform:sdl3
    include/pd/platform/window/sdl3/window_sdl3.hpp
    #platform:filesystem
    include/pd/platform/file/file_system.hpp
    include/pd/platform/file/std_file_system.hpp
    # platform:win32
    include/pd/platform/os/win32/platform_win32.hpp
    # backend
    include/pd/backend/backend.hpp
    include/pd/backend/backend_factory.hpp
    include/pd/backend/hw_enums.hpp
    include/pd/backend/hw_handle.hpp
    include/pd/backend/hw_swapchain.hpp
    include/pd/backend/hw_texture.hpp
    include/pd/backend/hw_buffer.hpp
    include/pd/backend/hw_command_recorder.hpp
    # resource
    include/pd/resource/resource_handle.hpp
    include/pd/resource/resource.hpp
    include/pd/resource/resource_manager.hpp
    include/pd/resource/resource/texture_resource.hpp
    include/pd/resource/resource/mesh_resource.hpp
    include/pd/resource/resource/material_resource.hpp
    include/pd/resource/resource_alias.hpp
    # rendering
    include/pd/rendering/layer/layer.hpp
    # include/pd/rendering/layer/engine_layer.hpp
    include/pd/rendering/renderer.hpp
    #include/pd/rendering/render_pass/render_pass.hpp
    #include/pd/rendering/render_pass/gbuffer_pass.hpp
    #include/pd/rendering/render_pass/forward_pass.hpp
    #include/pd/rendering/render_pass/present_pass.hpp
    include/pd/rendering/render_graph/render_graph.hpp
    # scene
    include/pd/scene/entity.hpp
    include/pd/scene/scene.hpp
    include/pd/scene/scene_descriptor.hpp
    include/pd/scene/scene_manager.hpp
    include/pd/scene/skybox.hpp
    include/pd/scene/view.hpp
    include/pd/scene/component/components.hpp
    # include/pd/scene/manager/components_registry.hpp
    include/pd/scene/manager/entity_manager.hpp
    # include/pd/scene/manager/transform_manager.hpp
    # include/pd/scene/manager/renderable_manager.hpp
    # include/pd/scene/manager/light_manager.hpp
    # include/pd/scene/manager/camera_manager.hpp
    # asset
    include/pd/asset/asset.hpp
    include/pd/asset/asset_parser.hpp
    include/pd/asset/asset_manager.hpp
    include/pd/asset/parser/gltf_parser.hpp
    include/pd/asset/parser/mesh_processor.hpp
    PRIVATE
    src/engine.cpp
    # core
    # platform
    src/platform/file/file_system.cpp
    src/platform/file/std_file_system.cpp
    # platform:sdl3
    src/platform/window/sdl3/window_sdl3.cpp
    # platform:win32
    src/platform/os/win32/platform_win32.cpp

    # rendering
    # src/rendering/layer/engine_layer.cpp
    src/rendering/renderer.cpp
    #src/rendering/render_pass/present_pass.cpp
    src/rendering/render_graph/render_graph.cpp
    # resource
    src/resource/resource_manager.cpp
    src/resource/resource/texture_resource.cpp
    src/resource/resource/mesh_resource.cpp
    # scene
    src/scene/scene.cpp
    src/scene/scene_manager.cpp
    src/scene/scene_descriptor.cpp
    # src/scene/manager/transform_manager.cpp
    # asset
    src/asset/asset_manager.cpp
    src/asset/parser/mesh_processor.cpp
    src/asset/parser/gltf_parser.cpp
)

if(${PD2_BACKEND} STREQUAL "VULKAN")
    target_sources(engine_lib
        PUBLIC
        include/pd/backend/vulkan/vulkan_common.hpp
        include/pd/backend/vulkan/vulkan_types.hpp
        include/pd/backend/vulkan/vulkan_context.hpp
        include/pd/backend/vulkan/vulkan_frame.hpp
        include/pd/backend/vulkan/vulkan_command_manager.hpp
        include/pd/backend/vulkan/backend_vulkan.hpp
        include/pd/backend/vulkan/resource/vulkan_resource_manager.hpp
        include/pd/backend/vulkan/resource/vulkan_resource.hpp
        include/pd/backend/vulkan/resource/vulkan_swapchain.hpp
        include/pd/backend/vulkan/resource/vulkan_image.hpp
        include/pd/backend/vulkan/resource/vulkan_command_buffer.hpp
        PRIVATE
        src/backend/vulkan/vulkan_context.cpp
        src/backend/vulkan/backend_vulkan.cpp
        src/backend/vulkan/vulkan_frame.cpp
        src/backend/vulkan/vulkan_command_manager.cpp
        src/backend/vulkan/resource/vulkan_resource_manager.cpp
        src/backend/vulkan/resource/vulkan_swapchain.cpp
        src/backend/vulkan/resource/vulkan_command_buffer.cpp
        # src/platform/rhi/vulkan/vulkan_swapchain.cpp
        #src/platform/rhi/vulkan/resource/vulkan_resource_manager.cpp
    )
    target_compile_definitions(engine_lib PRIVATE BACKEND_VULKAN)
endif()

if(${PD2_WINDOW} STREQUAL "SDL3")
    target_sources(engine_lib
        PUBLIC
        include/pd/platform/window/sdl3/window_sdl3.hpp
        PRIVATE
        src/platform/window/sdl3/window_sdl3.cpp
    )
    target_compile_definitions(engine_lib PRIVATE WINDOW_SDL3)
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

CompileShader(${PD2_SHADER_LANG} engine_lib examples/pyramid.slang pyramid.spv)

generate_export_header(engine_lib BASE_NAME pd2_engine)
configure_file(
    ${CMAKE_CURRENT_BINARY_DIR}/pd2_engine_export.h ${CMAKE_BINARY_DIR}/include/pd/pd2_engine_export.h
)

# add_dependencies(engine_lib pd2_shaders)

if(PD2_BUILD_TESTS)
    add_subdirectory(test)
endif()
