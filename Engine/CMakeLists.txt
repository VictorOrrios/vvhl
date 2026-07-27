include(${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/CompilerOptions.cmake)

file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS
    include/*.hpp
)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_library(Engine STATIC
    ${ENGINE_HEADERS}
    ${ENGINE_SOURCES}
)

set_project_warnings(Engine)
set_project_options(Engine)

target_precompile_headers(Engine
    PRIVATE
        include/vvhl/Core/Pch.hpp
)

target_include_directories(Engine
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/ThirdParty/renderdoc
    ${PROJECT_SOURCE_DIR}/ThirdParty/spirv-reflect
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Engine
PUBLIC
    glfw
    glm::glm
    fmt
    volk
    
    spdlog

    imgui
    imguizmo
    
    Tracy::TracyClient
    
)