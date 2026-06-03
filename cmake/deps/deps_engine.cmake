set(PD2_ENGINE_LIB_PUBLIC_LINKS)
# base
find_package(spdlog CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(fastgltf CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(meshoptimizer CONFIG REQUIRED)

list(APPEND PD2_ENGINE_LIB_PUBLIC_LINKS
    glm::glm-header-only
    spdlog::spdlog_header_only
    EnTT::EnTT
    fastgltf::fastgltf
    meshoptimizer::meshoptimizer
    ${Stb_INCLUDE_DIR}
)
# backend
if(PD2_BACKEND STREQUAL "VULKAN")
    find_package(VulkanHeaders CONFIG)
    find_package(volk CONFIG REQUIRED)
    find_package(VulkanMemoryAllocator CONFIG REQUIRED)
    find_package(VulkanUtilityLibraries CONFIG REQUIRED)
    find_package(vk-bootstrap CONFIG REQUIRED)
    
    list(APPEND PD2_ENGINE_LIB_PUBLIC_LINKS
        Vulkan::Headers
		GPUOpen::VulkanMemoryAllocator
		volk::volk
		vk-bootstrap::vk-bootstrap
    )
else()
    message(WARNING "USE_BACKEND 不符合要求，当前值是: ${PD2_BACKEND}")
endif()

# WSI
if(PD2_WSI STREQUAL "SDL3")
    find_package(SDL3 CONFIG REQUIRED)

    list(APPEND PD2_ENGINE_LIB_PUBLIC_LINKS
        SDL3::SDL3
    )
else()
    message(WARNING "PD2_WSI 不符合要求，当前值是: ${PD2_WSI}")
endif()

message(STATUS "ENGINE_PUBLIC_LINKS: ${PD2_ENGINE_LIB_PUBLIC_LINKS}")