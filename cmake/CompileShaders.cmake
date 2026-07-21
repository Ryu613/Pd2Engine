# use to compile shaders
set(shaderDir "${PD2_ROOT_DIR}/assets/shaders")
function(CompileShader lang target shader output)
if(lang STREQUAL "SLANG")
    find_program(SLANGC_EXECUTABLE slangc REQUIRED)
    add_custom_command(
        OUTPUT ${output}
        DEPENDS ${shaderDir}/${shader}
        COMMAND ${SLANGC_EXECUTABLE} ${shaderDir}/${shader} -target spirv -profile spirv_1_4 -entry vertMain -entry fragMain -o ${shaderDir}/${output}
        COMMENT "Compile Slang shader ${shader} using slangc"
    )
    add_custom_target(
        ${target}_shaders ALL
        DEPENDS ${output}
        COMMENT "Target to compile a slang shader"
    )
else()
    message(FATAL_ERROR "SHADER LANGUAGE NOT SUPPORTED!")
endif()
endfunction()

# add_custom_target(pd2_shaders ALL COMMENT "Target to copy the shader assets into the current source directory")

# file(GLOB_RECURSE SHADER_FILES ${shaderDir}*.spv)
# foreach(shader ${SHADER_FILES})
#     file(RELATIVE_PATH rel_path ${shaderDir} ${shader})
#     get_filename_component(rel_dir ${rel_path} DIRECTORY)
#     add_custom_command(
#         TARGET pd2_shaders
#         POST_BUILD
#         COMMAND ${CMAKE_COMMAND} 
#             -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/bin/shaders/${rel_dir}
#         COMMAND ${CMAKE_COMMAND} 
#             -E copy ${shader} ${CMAKE_CURRENT_BINARY_DIR}/bin/shaders/${rel_path}
#         COMMENT "Copy shader ${rel_path} into the binary directory"
#     )
# endforeach(shader)