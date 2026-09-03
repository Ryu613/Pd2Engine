cmake_minimum_required(VERSION 3.31)

add_library(pd_app)

target_sources(pd_app
    PUBLIC
        include/pd/app/application.hpp
    PRIVATE
        src/application.cpp
)

target_link_libraries(pd_app PRIVATE engine_lib)

target_include_directories(pd_app PUBLIC include PRIVATE src)