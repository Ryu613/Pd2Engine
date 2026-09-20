find_package(Catch2 CONFIG REQUIRED)

include(CTest)
include(Catch)

# build test program
function(add_engine_test NAME SOURCES)
    set(TARGET_NAME test_engine_${NAME})
    add_executable(${TARGET_NAME} ${SOURCES})
    target_compile_definitions(${TARGET_NAME} PRIVATE ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
    target_link_libraries(${TARGET_NAME} PRIVATE engine_lib Catch2::Catch2WithMain)

    add_test(NAME ${TARGET_NAME} COMMAND $<TARGET_FILE:${TARGET_NAME}>)
    set_tests_properties(${TARGET_NAME} PROPERTIES LABELS "Pd2Engine")
endfunction()
