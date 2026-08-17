add_library(imgui STATIC
    ${CMAKE_CURRENT_LIST_DIR}/../imgui/imgui.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../imgui/imgui_widgets.cpp

    # Optional
    ${CMAKE_CURRENT_LIST_DIR}/../imgui/imgui_demo.cpp

    ${CMAKE_CURRENT_LIST_DIR}/../imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../imgui/backends/imgui_impl_vulkan.cpp
)

target_compile_definitions(imgui PRIVATE 
    IMGUI_IMPL_VULKAN_USE_VOLK=1
    IMGUI_ENABLE_DOCKING=1
    IMGUI_ENABLE_VIEWPORT=1
)

target_include_directories(imgui
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../imgui
    ${CMAKE_CURRENT_LIST_DIR}/../imgui/backends
    ${CMAKE_CURRENT_LIST_DIR}/../volk
)

target_link_libraries(imgui
PUBLIC
    glfw
    Vulkan::Vulkan
)