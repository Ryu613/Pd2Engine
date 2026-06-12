find_package(Catch2 CONFIG REQUIRED)

include(CTest)
include(Catch)

# build test program
function(add_engine_test NAME SOURCES)
    set(TARGET_NAME test_engine_${NAME})
    add_executable(${TARGET_NAME} ${SOURCES})

    target_link_libraries(${TARGET_NAME} engine_lib Catch2::Catch2WithMain)

    add_test(NAME ${TARGET_NAME} COMMAND $<TARGET_FILE:${TARGET_NAME}>)
    set_tests_properties(${TARGET_NAME} PROPERTIES LABELS "Pd2Engine")
endfunction()