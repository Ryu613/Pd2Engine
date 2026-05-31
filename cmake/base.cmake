set(USE_FOLDERS ON)

set(PD2_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_compile_options("/utf-8")

if(WIN32)
    add_compile_definitions(NOMINMAX)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ${ipo_supported})
endif()
