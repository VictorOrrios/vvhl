function(set_project_warnings target)

if(MSVC)

target_compile_options(${target} PRIVATE

/W4

/WX

)

else()

target_compile_options(${target} PRIVATE

-Wall

-Wextra

-Wpedantic

-Werror

)

endif()

endfunction()