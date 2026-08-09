// Basic proyect import setup

// STL
#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <span>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// VULKAN
#include <volk.h>
// NOTE: Volk already loads vulkan.h, do not include

// VMA
#include <vma/vk_mem_alloc.h>

// SPIRV REFLECT
#include <spirv_reflect.h>

// GLTF
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// FMT
#include <fmt/base.h>
#include <fmt/color.h>
#include <fmt/os.h>
#include <fmt/ranges.h>

// CORE
#include <vvhl/Core/Assert.hpp>
#include <vvhl/Core/BuildConfig.hpp>
#include <vvhl/Core/EngineConfig.hpp>
#include <vvhl/Core/Logger.hpp>
#include <vvhl/Core/UUID.hpp>

#include <vvhl/Core/App.hpp>

#include <vvhl/Core/Input.hpp>
#include <vvhl/Core/Window.hpp>

// EVENTS
#include <vvhl/Events/Event.hpp>
#include <vvhl/Events/EventDispatcher.hpp>
#include <vvhl/Events/KeyboardEvents.hpp>
#include <vvhl/Events/MouseEvents.hpp>
#include <vvhl/Events/WindowEvents.hpp>

// SLOTMAP
#include <vvhl/SlotMap/SlotMap.hpp>

