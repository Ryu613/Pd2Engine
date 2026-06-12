cmake_minimum_required(VERSION 3.31)

# engine lib
add_library(engine_lib)
# 这句必须加，否则clangd无法识别c++20及以上语法(msvc clang-cl下)
# 原因在于std的latest无法正常识别，clangd的stl会fallback到C++14，导致stl14以上的stl和语法报错
# 可忽略构建时warning
# target_compile_options(engine_lib PUBLIC /std:c++23)
target_compile_features(engine_lib PUBLIC cxx_std_23)

#engine pch
target_precompile_headers(engine_lib PUBLIC include/pd/pch.hpp)

# 3rd-party libs
include(${PD2_ROOT_DIR}/cmake/deps/EngineDeps.cmake)

# 第三方，稳定，且无vcpkg，和cmake支持的库
# 比自己写vcpkg port或cmake打包方便，直接一起打包
file(GLOB_RECURSE vendor_src CONFIGURE_DEPENDS vendor/*.h vendor/*.hpp vendor/*.c vendor/*.cpp)

add_library(vendor_lib STATIC ${vendor_src})

target_include_directories(vendor_lib PUBLIC vendor)

target_link_libraries(engine_lib
    PUBLIC
        ${PD2_ENGINE_LIB_PUBLIC_LINKS}
        vendor_lib
)

target_sources(engine_lib
    PUBLIC
        # headers
        include/pd/engine.hpp
        # core
        include/pd/core/globals.hpp
        include/pd/core/logging.hpp
        include/pd/core/assert.hpp
        include/pd/core/handle.hpp
        include/pd/core/entity.hpp
        include/pd/core/utils/memory.hpp
        include/pd/core/allocators.hpp
        # platform
        include/pd/platform/platform.hpp
        include/pd/platform/rhi/rhi_api.hpp
        include/pd/platform/rhi/rhi_handle.hpp
        include/pd/platform/rhi/rhi_resource.hpp
        include/pd/platform/platform_factory.hpp
        include/pd/platform/windows/platform_windows.hpp
        include/pd/platform/file/file_system.hpp
        include/pd/platform/file/std_file_system.hpp
        # resource
        include/pd/resource/resource_handle.hpp
        include/pd/resource/resource.hpp
        include/pd/resource/resource_manager.hpp
        # rendering
        include/pd/rendering/renderer.hpp
        include/pd/rendering/resource/texture_resource.hpp
        include/pd/rendering/resource/mesh_resource.hpp
        include/pd/rendering/resource/material_resource.hpp
        # scene
        include/pd/scene/scene.hpp
        include/pd/scene/scene_descriptor.hpp
        include/pd/scene/skybox.hpp
        include/pd/scene/view.hpp
        include/pd/scene/component/components.hpp
        include/pd/scene/manager/transform_manager.hpp
        include/pd/scene/manager/renderable_manager.hpp
        include/pd/scene/manager/light_manager.hpp
        include/pd/scene/manager/camera_manager.hpp
        # asset
        include/pd/asset/asset.hpp
        include/pd/asset/asset_parser.hpp
        include/pd/asset/asset_manager.hpp
        include/pd/asset/parser/gltf_parser.hpp
        include/pd/asset/parser/mesh_processor.hpp
    PRIVATE
        src/engine.cpp
        # core
        src/core/utils/memory.cpp
        # platform
        src/platform/windows/platform_windows.cpp
        src/platform/file/file_system.cpp
        src/platform/file/std_file_system.cpp
        # rendering
        src/rendering/renderer.cpp
        src/rendering/resource/texture_resource.cpp
        src/rendering/resource/mesh_resource.cpp
        # resource
        src/resource/resource_manager.cpp
        # scene
        src/scene/scene.cpp
        # asset
        src/asset/asset_manager.cpp
        src/asset/parser/mesh_processor.cpp
        src/asset/parser/gltf_parser.cpp
)

if(${PD2_BACKEND} STREQUAL "VULKAN")
target_sources(engine_lib
    PUBLIC
        include/pd/platform/rhi/vulkan/rhi_vulkan.hpp
		include/pd/platform/rhi/vulkan/vulkan_context.hpp
		include/pd/platform/rhi/vulkan/vulkan_swapchain.hpp
		include/pd/platform/rhi/vulkan/vulkan_frame.hpp
		include/pd/platform/rhi/vulkan/resource/vulkan_resource_manager.hpp
    PRIVATE
        src/platform/rhi/vulkan/rhi_vulkan.cpp
		src/platform/rhi/vulkan/vulkan_context.cpp
		src/platform/rhi/vulkan/vulkan_swapchain.cpp
		src/platform/rhi/vulkan/vulkan_frame.cpp
		src/platform/rhi/vulkan/resource/vulkan_resource_manager.cpp
)
endif()

target_include_directories(engine_lib
    PUBLIC
        include
        ${PD2_ENGINE_LIB_PUBLIC_INCLUDE}
    PRIVATE
        src
)

if(PD2_BUILD_TESTS)
    add_subdirectory(test)
endif()