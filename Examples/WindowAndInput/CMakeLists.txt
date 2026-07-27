add_executable(WindowAndInput
    main.cpp
)

set_project_warnings(WindowAndInput)
set_project_options(WindowAndInput)

target_link_libraries(WindowAndInput
    PRIVATE Engine
)