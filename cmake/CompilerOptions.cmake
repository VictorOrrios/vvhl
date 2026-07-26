function(set_project_options target)

    target_compile_features(${target}
        PUBLIC cxx_std_20
    )

    target_compile_definitions(${target}
        PRIVATE
            $<$<CONFIG:Debug>:BUILD_DEBUG>
            $<$<CONFIG:Release>:BUILD_RELEASE>
            $<$<CONFIG:RelWithDebInfo>:BUILD_DEV>
    )
    
endfunction()