# use to compile shaders
function(CompileShader lang target shader output)
set(shaderDir "${PD2_ROOT_DIR}/assets/shaders/")
if(lang STREQUAL "SLANG")
    find_program(SLANGC_EXECUTABLE slangc REQUIRED)
    add_custom_command(
        OUTPUT ${output}
        DEPENDS ${PD2_ROOT_DIR}/assets/shaders/${shader}
        COMMAND ${SLANGC_EXECUTABLE} ${shaderDir}${shader} -target spirv -profile spirv_1_4 -entry vertMain -entry fragMain -o ${shaderDir}${output}
        COMMENT "Compile Slang shader ${shader} using slangc"
    )
    add_custom_target(
        ${target}Shaders ALL
        DEPENDS ${output}
        COMMENT "Target to compile a slang shader"
    )
else()
    message(FATAL_ERROR "SHADER LANGUAGE NOT SUPPORTED!")
endif()
endfunction()