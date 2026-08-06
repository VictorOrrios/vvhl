#pragma once

#include <vvhl/vvhl.hpp>

class GLFWContext {
public:
  GLFWContext();
  ~GLFWContext();

  GLFWContext(const GLFWContext &) = delete;
  GLFWContext &operator=(const GLFWContext &) = delete;

  static std::vector<const char *> getRequiredExtensions() {
    uint32_t count = 0;
    const char **ext = glfwGetRequiredInstanceExtensions(&count);
    return std::vector<const char *>(ext, ext + count);
  }
};