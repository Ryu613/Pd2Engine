# TODO
function(pd2_add_sample sample_name)
    add_subdirectory(${sample_name})
    set(target_name ${sample_name}_sample)
    add_executable(${target_name} ${sample_name}/app.cpp)
    target_link_libraries(${target_name} PRIVATE ${sample_name}_lib)
endfunction()