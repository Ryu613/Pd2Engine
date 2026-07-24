add_library(vendor_lib)
target_sources(vendor_lib
    PUBLIC
    vendor/imgui/imconfig.h
    vendor/imgui/imgui.h
    vendor/imgui/imgui_internal.h
    vendor/imgui/imstb_rectpack.h
    vendor/imgui/imstb_textedit.h
    vendor/imgui/imstb_truetype.h
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/tlsf/tlsf.h
    vendor/tlsf/tlsf.c
    vendor/tsl/robin_growth_policy.h
    vendor/tsl/robin_hash.h
    vendor/tsl/robin_map.h
    vendor/tsl/robin_set.h
)

if(PD2_BACKEND STREQUAL "VULKAN")
    find_package(VulkanHeaders CONFIG REQUIRED)
    target_sources(vendor_lib
        PUBLIC
        vendor/imgui/imgui_impl_vulkan.h
        vendor/imgui/imgui_impl_vulkan.cpp
    )
    target_link_libraries(vendor_lib PRIVATE Vulkan::Headers)
    target_compile_definitions(vendor_lib PRIVATE
        IMGUI_IMPL_VULKAN_NO_PROTOTYPES
        IMGUI_IMPL_VULKAN_USE_VOLK
    )
endif()

target_include_directories(vendor_lib PUBLIC vendor)

if(PD2_WINDOW STREQUAL "SDL3")
endif()
