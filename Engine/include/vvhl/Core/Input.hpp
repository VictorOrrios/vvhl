#pragma once
#include <vvhl/Events/EventDispatcher.hpp>

struct GLFWwindow;

namespace vvhl {

class Input {
public:
  enum class ButtonState {
    Released = GLFW_RELEASE,
    Pressed = GLFW_PRESS,
    Repeated = GLFW_REPEAT
  };

  // App
  static void initialize(EventDispatcher &dispatcher, GLFWwindow *window);
  static void endFrame();

  // Keyboard
  static bool isKeyPressed(int key);

  // Mouse
  static bool isMouseButtonPressed(int button);
  static glm::dvec2 getMousePosition();
  static glm::dvec2 getMouseDelta();
  static glm::dvec2 getScrollDelta();

  // Cursor
  static void showCursor();
  static void hideCursor();
  static void lockCursor();
  static void unlockCursor();
  static void setCursorPosition(double x, double y);

private:
  // Callback init
  static void registerCallbacks(GLFWwindow *window);

  static void KeyCallback(GLFWwindow *, int key, int scancode, int action,
                          int mods);

  static void CharCallback(GLFWwindow *, unsigned int codepoint);

  static void MouseButtonCallback(GLFWwindow *, int button, int action,
                                  int mods);

  static void CursorPositionCallback(GLFWwindow *, double x, double y);

  static void CursorEnterCallback(GLFWwindow *, int entered);

  static void ScrollCallback(GLFWwindow *, double xOffset, double yOffset);

private:
  inline static EventDispatcher *s_dispatcher = nullptr;
  inline static GLFWwindow *s_window = nullptr;
  inline static std::array<ButtonState, GLFW_KEY_LAST + 1> s_keys{};
  inline static std::array<ButtonState, GLFW_MOUSE_BUTTON_LAST + 1>
      s_mouseButtons{};
  inline static glm::dvec2 s_mousePosition{0.0};
  inline static glm::dvec2 s_previousMousePosition{0.0};
  inline static glm::dvec2 s_mouseDelta{0.0};
  inline static glm::dvec2 s_scrollDelta{0.0};
};

} // namespace vvhl