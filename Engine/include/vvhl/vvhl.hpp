// Basic proyect import setup

// STL
#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include <typeindex>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Vulkan
#include <volk.h>
#include <vulkan/vulkan.hpp>

// GLTF
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// FMT
#include <fmt/base.h>
#include <fmt/ranges.h>
#include <fmt/os.h>
#include <fmt/color.h>

// CORE
#include <vvhl/Core/Logger.hpp>
#include <vvhl/Core/Config.hpp>
#include <vvhl/Core/Assert.hpp>
#include <vvhl/Core/UUID.hpp>

#include <vvhl/Core/App.hpp>

#include <vvhl/Core/Window.hpp>
#include <vvhl/Core/Input.hpp>

#include <vvhl/Events/Event.hpp>
#include <vvhl/Events/EventDispatcher.hpp>
#include <vvhl/Events/WindowEvents.hpp>
#include <vvhl/Events/KeyboardEvents.hpp>
#include <vvhl/Events/MouseEvents.hpp>
