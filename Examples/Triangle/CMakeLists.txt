add_executable(Triangle
    main.cpp
)

set_project_warnings(Triangle)
set_project_options(Triangle)

target_link_libraries(Triangle
    PRIVATE Engine
)