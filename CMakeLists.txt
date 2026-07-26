cmake_minimum_required(VERSION 3.25)

find_package(Vulkan REQUIRED)

find_program(GLSLC_EXECUTABLE
    NAMES glslc
    HINTS
        ENV VULKAN_SDK
    PATH_SUFFIXES Bin bin
)

if(NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc not found. Please install the Vulkan SDK.")
endif()

find_program(SLANGC_EXECUTABLE
    NAMES slangc
)

if(NOT SLANGC_EXECUTABLE)
    message(WARNING "slangc not found. Slang shaders will not be available.")
endif()

project(
    VVHL
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(ThirdParty)
add_subdirectory(Engine)
add_subdirectory(Examples)