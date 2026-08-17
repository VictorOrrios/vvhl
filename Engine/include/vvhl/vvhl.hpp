// Basic proyect import setup

// STL
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <span>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <variant>
#include <vector>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// VULKAN
#include <volk.h>
// NOTE: Volk already loads vulkan.h, do not include
#include <vulkan/vk_enum_string_helper.h>

// VMA
#include <vma/vk_mem_alloc.h>

// SPIRV REFLECT
#include <spirv_reflect.h>

// GLFW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// FMT
#include <fmt/base.h>
#include <fmt/color.h>
#include <fmt/os.h>
#include <fmt/ranges.h>

// IMGUI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

// CORE
#include <vvhl/Core/Assert.hpp>
#include <vvhl/Core/BuildConfig.hpp>
#include <vvhl/Core/EngineConfig.hpp>
#include <vvhl/Core/Logger.hpp>
#include <vvhl/Core/UUID.hpp>

// SLOTMAP
#include <vvhl/SlotMap/SlotMap.hpp>

