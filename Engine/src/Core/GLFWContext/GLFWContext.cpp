
#include "GLFWContext.hpp"
#include <GLFW/glfw3.h>

namespace vvhl {

GLFWContext::GLFWContext() {
  if (!glfwInit())
    throw std::runtime_error("Failed to initialize GLFW");
}

GLFWContext::~GLFWContext() { glfwTerminate(); }
} // namespace vvhl
