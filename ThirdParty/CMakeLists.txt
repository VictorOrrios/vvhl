find_package(Vulkan REQUIRED)

add_subdirectory(fmt)
add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(volk)
add_subdirectory(spdlog)
add_subdirectory(tracy)
add_subdirectory(imguizmo)

include(cmake/imgui.cmake)

add_library(spirv-reflect STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/spirv-reflect/spirv_reflect.c
)

target_include_directories(spirv-reflect
  PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/spirv-reflect
)