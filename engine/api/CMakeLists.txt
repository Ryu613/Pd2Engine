
add_library(EngineApi)

target_sources(EngineApi
 PUBLIC
    include/pd/api.hpp
    include/pd/engine.hpp
)

target_include_directories(EngineApi PUBLIC include)