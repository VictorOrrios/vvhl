function(set_project_options target)

target_compile_features(${target}

PUBLIC

cxx_std_20

)

endfunction()