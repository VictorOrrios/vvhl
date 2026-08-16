#pragma once

#include <vvhl/Core/Logger.hpp>

namespace vvhl {

class GLFWContext {
public:
  static bool init() {
    if (!glfwInit()) {
      LOGE("Failed to initialize glfw")
      return false;
    }
    return true;
  }

  static void terminate() { glfwTerminate(); }

  static std::vector<const char *> getRequiredExtensions() {
    uint32_t count = 0;
    const char **ext = glfwGetRequiredInstanceExtensions(&count);
    return std::vector<const char *>(ext, ext + count);
  }
};

} // namespace vvhl
