# 3rd party libs
set(PD2_ENGINE_LIB_PUBLIC_LINKS)
# 3rd party include dir(header only)
set(PD2_ENGINE_LIB_PUBLIC_INCLUDE)


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
)
list(APPEND PD2_ENGINE_LIB_PUBLIC_INCLUDE
    ${Stb_INCLUDE_DIR}
)
# backend
if(PD2_BACKEND STREQUAL "VULKAN")
    find_package(VulkanHeaders CONFIG REQUIRED)
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

# window system
if(PD2_WINDOW STREQUAL "SDL3")
    find_package(SDL3 CONFIG REQUIRED)

    list(APPEND PD2_ENGINE_LIB_PUBLIC_LINKS
        SDL3::SDL3
    )
else()
    message(WARNING "PD2_WINDOW 不符合要求，当前值是: ${PD2_WINDOW}")
endif()

# shader language
if(PD2_SHADER_LANG STREQUAL "SLANG")
    find_package(slang CONFIG REQUIRED)

    list(APPEND PD2_ENGINE_LIB_PUBLIC_LINKS
        slang::slang
    )
else()
    message(WARNING "PD2_SHADER_LANG 不符合要求，当前值是: ${PD2_SHADER_LANG}")
endif()

message(STATUS "ENGINE_PUBLIC_LINKS: ${PD2_ENGINE_LIB_PUBLIC_LINKS}")