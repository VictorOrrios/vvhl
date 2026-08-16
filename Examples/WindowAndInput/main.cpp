// Window and input example

// TODO: Restore to functionality

/*

#include "vvhl/Core/GLFWContext.hpp"
#include <vvhl/vvhl.hpp>

using namespace vvhl;

int main() {
  Logger::Init();

  GLFWContext glfw;

  EventDispatcher dispatcher;

  Window window;
  window.initialize(
      {
          .Title = "VVHL Sandbox",
          .Width = 1280,
          .Height = 720,
          .Resizable = true,
      },
      dispatcher);

  dispatcher.subscribe<WindowResizeEvent>([](const WindowResizeEvent &e) {
    LOGI("Resize {}x{}", e.Width, e.Height);
  });

  dispatcher.subscribe<FramebufferResizeEvent>(
      [](const FramebufferResizeEvent &e) {
        LOGI("Framebuffer {}x{}", e.Width, e.Height);
      });

  dispatcher.subscribe<WindowMoveEvent>(
      [](const WindowMoveEvent &e) { LOGI("Move {} {}", e.X, e.Y); });

  dispatcher.subscribe<WindowFocusEvent>(
      [](const WindowFocusEvent &e) { LOGI("Focus {}", e.Focused); });

  dispatcher.subscribe<WindowMinimizeEvent>(
      [](const WindowMinimizeEvent &e) { LOGI("Minimized {}", e.Minimized); });

  dispatcher.subscribe<WindowMaximizeEvent>(
      [](const WindowMaximizeEvent &e) { LOGI("Maximized {}", e.Maximized); });

  dispatcher.subscribe<WindowCloseEvent>([&](const WindowCloseEvent &) {
    LOGI("Close requested");
    window.close();
  });

  dispatcher.subscribe<KeyPressedEvent>([&](const KeyPressedEvent &e) {
    LOGI("Key {} pressed", e.Key);

    switch (e.Key) {
    case GLFW_KEY_ESCAPE:
      window.close();
      break;

    case GLFW_KEY_H:
      Input::hideCursor();
      break;

    case GLFW_KEY_S:
      Input::showCursor();
      break;

    case GLFW_KEY_L:
      Input::lockCursor();
      break;

    case GLFW_KEY_U:
      Input::unlockCursor();
      break;

    case GLFW_KEY_V:
      Input::setCursorPosition(0,0);
      break;

    case GLFW_KEY_M:
      window.maximize();
      break;

    case GLFW_KEY_N:
      window.minimize();
      break;

    case GLFW_KEY_P:
      window.setPosition(0, 0);
      break;
    
    case GLFW_KEY_F:
      window.setFullscreen(!window.isFullscreen());
      break;

    case GLFW_KEY_A:
      window.setSize(30, 30);
      break;

    case GLFW_KEY_R:
      LOGI("Window restored")
      window.restore();
      break;
    }
  });

  dispatcher.subscribe<KeyReleasedEvent>(
      [](const KeyReleasedEvent &e) { LOGI("Key {} released", e.Key); });

  dispatcher.subscribe<CharInputEvent>([](const CharInputEvent &e) {
    LOGI("Character {}", static_cast<char>(e.Codepoint));
  });

  //dispatcher.subscribe<MouseMovedEvent>(
  //    [](const MouseMovedEvent &e) { LOGD("Mouse {:.0f} {:.0f}", e.X, e.Y); });

  dispatcher.subscribe<MouseScrolledEvent>([](const MouseScrolledEvent &e) {
    LOGI("Scroll {:.1f} {:.1f}", e.XOffset, e.YOffset);
  });

  dispatcher.subscribe<MouseButtonPressedEvent>(
      [](const MouseButtonPressedEvent &e) {
        LOGI("Mouse {} Press ({:.0f}, {:.0f})", e.Button, e.X, e.Y);
      });

  dispatcher.subscribe<MouseButtonReleasedEvent>(
      [](const MouseButtonReleasedEvent &e) {
        LOGI("Mouse {} Release ({:.0f}, {:.0f})", e.Button, e.X, e.Y);
      });

  while (window.isOpen()) {
    window.pollEvents();

    if (Input::isKeyPressed(GLFW_KEY_SPACE))
      LOGD("Holding SPACE");

    auto delta = Input::getMouseDelta();

    if (delta.x != 0.0 || delta.y != 0.0) {
      //LOGD("Delta {:.1f} {:.1f}", delta.x, delta.y);
    }


    glfwSwapBuffers(window.getNativeHandle());


    Input::endFrame();
  }

  return 0;
}

*/
int main() {
  return 0;
}