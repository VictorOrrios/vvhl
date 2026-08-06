
#include "GLFWContext.hpp"
#include <GLFW/glfw3.h>

GLFWContext::GLFWContext() {
  if (!glfwInit())
    throw std::runtime_error("Failed to initialize GLFW");
}

GLFWContext::~GLFWContext() { glfwTerminate(); }