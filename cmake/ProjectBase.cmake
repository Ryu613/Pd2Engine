set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(PD2_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "project root dir")
set(PD2_ASSET_DIR "${PD2_ROOT_DIR}/assets" CACHE PATH "project asset dir")
set(PD2_SHADER_DIR "${PD2_ASSET_DIR}/shaders" CACHE PATH "project shader dir")

add_compile_options("/utf-8")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(WIN32)
    add_compile_definitions(NOMINMAX)
    add_compile_definitions(WIN32_LEAN_AND_MEAN)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ${ipo_supported})
endif()
