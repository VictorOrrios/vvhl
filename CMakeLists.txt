cmake_minimum_required(VERSION 3.25)

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