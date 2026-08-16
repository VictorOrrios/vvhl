#include <vvhl/Core/App.hpp>
#include <vvhl/Core/GLFWContext.hpp>
#include <vvhl/Core/Window.hpp>
#include <vvhl/Events/WindowEvents.hpp>

namespace vvhl {

bool App::initializeBase(const AppConfig &config) {

  GLFWContext::init();

  m_eventDispatcher.subscribe<WindowCloseEvent>(
      [this](const WindowCloseEvent &) { m_shouldClose = true; });

  m_window.initialize(config.windowSpec, m_eventDispatcher);

  if (!m_context.initialize(m_window)) {
    destroy();
    return false;
  }

  if (!m_resourceManager.initialize(m_context)) {
    destroy();
    return false;
  }

  if(!m_cmdSystem.initialize(m_context.device())){
    destroy();
    return false;
  }
  m_cmdPool = &m_cmdSystem.graphicsPool();

  if (!m_frameManager.initialize(m_context, *m_cmdPool)) {
    destroy();
    return false;
  }

  return true;
}

void App::destroyBase() {
  m_context.device().waitIdle();
  m_frameManager.destroy();
  m_cmdSystem.destroy();
  m_cmdPool = nullptr;
  m_resourceManager.destroy();
  m_context.destroy();
  m_window.destroy();
  m_eventDispatcher.destroy();
  m_shouldClose = false;

  GLFWContext::terminate();
}

void App::run() {
  FrameManager::Frame *f;
  VkImageView outputView;
  while (!m_shouldClose) {
    // Poll glfw events
    m_window.pollEvents();

    // Wait fence, begin cmd, acquire image
    if (!m_frameManager.beginFrame(f, outputView)) {
      destroy();
      return;
    }

    // Record cmd
    onRender(f->cmdBuffer.handle(), outputView, f->frameNumber);

    // Queue submit cmd, present swapchain img, end frame
    if (!m_frameManager.endFrame()) {
      destroy();
      return;
    }
  }
}

} // namespace vvhl