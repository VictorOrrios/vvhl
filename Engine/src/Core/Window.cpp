
#include <vvhl/Core/Input.hpp>
#include <vvhl/Core/Window.hpp>
#include <vvhl/Events/WindowEvents.hpp>
#include <vvhl/vvhl.hpp>

namespace vvhl {

static Window *getWindow(GLFWwindow *window) {
  return static_cast<Window *>(glfwGetWindowUserPointer(window));
}

Window::Window(WindowSpecification spec, vvhl::EventDispatcher &dispatcher)
    : m_spec(std::move(spec)), m_dispatcher(dispatcher) {}

void Window::create() {
  // glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  //  TODO: Vulkan is GLFW_NO_API, OpenGL is GLFW_OPENGL_API
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_RESIZABLE, m_spec.Resizable);

  m_glfwHandle = glfwCreateWindow(m_spec.Width, m_spec.Height,
                                  m_spec.Title.c_str(), nullptr, nullptr);

  if (!m_glfwHandle)
    throw std::runtime_error("Failed to create window");

  // TODO: Delete this, this is for OpenGL basic setup
  glfwMakeContextCurrent(m_glfwHandle);
  glfwSwapInterval(1);

  glfwSetWindowUserPointer(m_glfwHandle, this);

  // Register window callbacks
  registerCallbacks();

  // Initialize and register input callbacks
  vvhl::Input::initialize(m_dispatcher, m_glfwHandle);
}

void Window::destroy() {
  if (m_glfwHandle) {
    glfwDestroyWindow(m_glfwHandle);
    m_glfwHandle = nullptr;
  }
}

void Window::close() { glfwSetWindowShouldClose(m_glfwHandle, GLFW_TRUE); }

void Window::pollEvents() { glfwPollEvents(); }

bool Window::isOpen() const { return !glfwWindowShouldClose(m_glfwHandle); }
bool Window::isFocused() const {
  return glfwGetWindowAttrib(m_glfwHandle, GLFW_FOCUSED);
}
bool Window::isMinimized() const {
  return glfwGetWindowAttrib(m_glfwHandle, GLFW_ICONIFIED);
}
bool Window::isFullscreen() const {
  return glfwGetWindowMonitor(m_glfwHandle) != nullptr;
}

void Window::setTitle(const std::string &title) {
  m_spec.Title = title;

  glfwSetWindowTitle(m_glfwHandle, title.c_str());
}
void Window::setSize(uint32_t width, uint32_t height) {
  m_spec.Width = width;
  m_spec.Height = height;

  glfwSetWindowSize(m_glfwHandle, width, height);
}
void Window::setPosition(int x, int y) { glfwSetWindowPos(m_glfwHandle, x, y); }
void Window::setFullscreen(bool enabled) {
  if (enabled == m_isFullscreen)
    return;

  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);

  if (enabled) {
    // Save current window state
    glfwGetWindowPos(m_glfwHandle, &m_windowedState.x, &m_windowedState.y);
    glfwGetWindowSize(m_glfwHandle, &m_windowedState.width,
                      &m_windowedState.height);

    glfwSetWindowMonitor(m_glfwHandle, monitor, 0, 0, mode->width, mode->height,
                         mode->refreshRate);
  } else {
    glfwSetWindowMonitor(m_glfwHandle, nullptr, m_windowedState.x,
                         m_windowedState.y, m_windowedState.width,
                         m_windowedState.height, 0);
  }

  m_isFullscreen = enabled;
}

VkExtent2D Window::getFramebufferSize() const {
  int w, h;

  glfwGetFramebufferSize(m_glfwHandle, &w, &h);

  return {static_cast<uint32_t>(w), static_cast<uint32_t>(h)};
}
uint32_t Window::getWidth() const { return getFramebufferSize().width; };
uint32_t Window::getHeight() const { return getFramebufferSize().height; };

GLFWwindow *Window::getNativeHandle() const { return m_glfwHandle; }
glm::ivec2 Window::getPosition() const {
  int x, y;
  glfwGetWindowPos(m_glfwHandle, &x, &y);

  return glm::ivec2(x, y);
}

void Window::maximize() { glfwMaximizeWindow(m_glfwHandle); }
void Window::minimize() { glfwIconifyWindow(m_glfwHandle); }
void Window::restore() { glfwRestoreWindow(m_glfwHandle); }

VkSurfaceKHR Window::createSurface(VkInstance instance) const {
  VkSurfaceKHR surface;

  if (glfwCreateWindowSurface(instance, m_glfwHandle, nullptr, &surface) !=
      VK_SUCCESS) {
    throw std::runtime_error("Couldn't create window surface");
  }

  return surface;
}

void Window::registerCallbacks() {
  glfwSetWindowSizeCallback(m_glfwHandle, WindowSizeCallback);

  glfwSetFramebufferSizeCallback(m_glfwHandle, FramebufferSizeCallback);

  glfwSetWindowCloseCallback(m_glfwHandle, WindowCloseCallback);

  glfwSetWindowFocusCallback(m_glfwHandle, WindowFocusCallback);

  glfwSetWindowIconifyCallback(m_glfwHandle, WindowIconifyCallback);

  glfwSetWindowMaximizeCallback(m_glfwHandle, WindowMaximizeCallback);

  glfwSetWindowPosCallback(m_glfwHandle, WindowPositionCallback);
}

void Window::WindowSizeCallback(GLFWwindow *window, int width, int height) {
  auto *self = getWindow(window);

  self->m_spec.Width = static_cast<uint32_t>(width);
  self->m_spec.Height = static_cast<uint32_t>(height);
  self->m_dispatcher.dispatch(
      vvhl::WindowResizeEvent(self, self->m_spec.Width, self->m_spec.Height));
}

void Window::FramebufferSizeCallback(GLFWwindow *window, int width,
                                     int height) {
  auto *self = getWindow(window);

  self->m_dispatcher.dispatch(vvhl::FramebufferResizeEvent(
      self, static_cast<uint32_t>(width), static_cast<uint32_t>(height)));
}

void Window::WindowCloseCallback(GLFWwindow *window) {
  auto *self = getWindow(window);

  self->m_dispatcher.dispatch(vvhl::WindowCloseEvent(self));
}

void Window::WindowFocusCallback(GLFWwindow *window, int focused) {
  auto *self = getWindow(window);

  self->m_dispatcher.dispatch(
      vvhl::WindowFocusEvent(self, focused == GLFW_TRUE));
}

void Window::WindowIconifyCallback(GLFWwindow *window, int iconified) {
  auto *self = getWindow(window);

  self->m_dispatcher.dispatch(
      vvhl::WindowMinimizeEvent(self, iconified == GLFW_TRUE));
}

void Window::WindowMaximizeCallback(GLFWwindow *window, int maximized) {
  auto *self = getWindow(window);

  self->m_dispatcher.dispatch(
      vvhl::WindowMaximizeEvent(self, maximized == GLFW_TRUE));
}

void Window::WindowPositionCallback(GLFWwindow *window, int x, int y) {
  auto *self = getWindow(window);

  self->m_dispatcher.dispatch(vvhl::WindowMoveEvent(self, x, y));
}

}; // namespace vvhl
