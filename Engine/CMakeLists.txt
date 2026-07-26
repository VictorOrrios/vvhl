include(${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/CompilerOptions.cmake)


file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_library(Engine STATIC ${ENGINE_SOURCES})

set_project_warnings(Engine)
set_project_options(Engine)

target_precompile_headers(Engine
    PRIVATE
        include/Core/pch.hpp
)

target_include_directories(Engine
PUBLIC
    include
PRIVATE
    src
)

target_link_libraries(Engine
PUBLIC
    glfw
    ${PROJECT_SOURCE_DIR}/ThirdParty/glm
    fmt
    volk
    shaderc
    spirv-reflect
    spdlog

    imgui
    imguizmo
    
    tracy
    ${PROJECT_SOURCE_DIR}/ThirdParty/renderdoc
)