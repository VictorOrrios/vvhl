
set(SHADERC_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)


add_subdirectory(fmt)
add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(volk)
add_subdirectory(spdlog)
add_subdirectory(tracy)
add_subdirectory(imguizmo)
add_subdirectory(imgui)
