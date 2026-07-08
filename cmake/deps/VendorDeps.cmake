set(vendor_dir "${PD2_ROOT_DIR}/engine/vendor")
add_library(vendor_lib)
target_sources(vendor_lib
    PUBLIC
        ${vendor_dir}/imgui/imconfig.h
        ${vendor_dir}/imgui/imgui.h
        ${vendor_dir}/imgui/imgui_internal.h
        ${vendor_dir}/imgui/imstb_rectpack.h
        ${vendor_dir}/imgui/imstb_textedit.h
        ${vendor_dir}/imgui/imstb_truetype.h
        ${vendor_dir}/imgui/imgui.cpp
        ${vendor_dir}/imgui/imgui_draw.cpp
        ${vendor_dir}/imgui/imgui_demo.cpp
        ${vendor_dir}/imgui/imgui_tables.cpp
        ${vendor_dir}/imgui/imgui_widgets.cpp
)

if(PD2_BACKEND STREQUAL "VULKAN")
target_sources(vendor_lib
    PUBLIC
        ${vendor_dir}/imgui/imgui_impl_vulkan.h
        ${vendor_dir}/imgui/imgui_impl_vulkan.cpp
)
target_link_libraries(vendor_lib PRIVATE Vulkan::Headers)
target_compile_definitions(vendor_lib PRIVATE
    IMGUI_IMPL_VULKAN_NO_PROTOTYPES
    IMGUI_IMPL_VULKAN_USE_VOLK
)
endif()

if(PD2_WINDOW STREQUAL "SDL3")
endif()

set(PD2_VENDOR_LIB_PUBLIC_INCLUDE ${vendor_dir})