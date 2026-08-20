#include <vvhl/Core/App.hpp>
#include <vvhl/Core/GLFWContext.hpp>
#include <vvhl/Core/Window.hpp>
#include <vvhl/Events/WindowEvents.hpp>

namespace vvhl {

bool App::initializeBase(const AppConfig &config) {
  Logger::init();

  GLFWContext::init();
  LOGI("Initialized: GLFW Context")

  m_eventDispatcher.subscribe<WindowCloseEvent>(
      [this](const WindowCloseEvent &) { m_shouldClose = true; });

  m_window.initialize(config.windowSpec, m_eventDispatcher);
  LOGI("Initialized: Window")

  if (!m_context.initialize(m_window)) {
    destroy();
    return false;
  }
  LOGI("Initialized: Vulkan context")

  if (!m_resourceManager.initialize(m_context)) {
    destroy();
    return false;
  }
  LOGI("Initialized: Resource manager")

  if (!m_cmdSystem.initialize(m_context.device())) {
    destroy();
    return false;
  }
  m_cmdPool = &m_cmdSystem.graphicsPool();
  LOGI("Initialized: Command system")

  if (!m_frameManager.initialize(m_context, *m_cmdPool)) {
    destroy();
    return false;
  }
  LOGI("Initialized: Frame manager")

  if (!m_imguiLayer.initialize(m_context, m_window)) {
    destroy();
    return false;
  }
  LOGI("Initialized: ImGui Layer")

  if (!createViewport()) {
    destroy();
    return false;
  }
  LOGI("Initialized: Viewport")

  return true;
}

bool App::createViewport() {
  // TODO: Change to dynamic viewport resizing
  VkExtent2D viewportSize =
      VkExtent2D(m_window.getWidth(), m_window.getHeight());
  LOGD("Viewport size {}x{}", viewportSize.width, viewportSize.height)

  m_viewport = m_resourceManager.createImage(
      {.format = VK_FORMAT_B8G8R8A8_UNORM,
       .width = viewportSize.width,
       .height = viewportSize.height,
       .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | // Graphics pipelines
        VK_IMAGE_USAGE_STORAGE_BIT |                  // Compute pipelines
        VK_IMAGE_USAGE_SAMPLED_BIT |                  // ImGui sampling
        VK_IMAGE_USAGE_TRANSFER_DST_BIT |             // To blit/copy if necesary
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT,              // To read back for debugging or exports
        .aspectMask=VK_IMAGE_ASPECT_COLOR_BIT,
      });

  m_viewportSampler = m_resourceManager.createSampler({
      .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
  });

  m_viewportSet = m_imguiLayer.createViewportTextureId(
      m_resourceManager.image(m_viewport).view(),
      m_resourceManager.sampler(m_viewportSampler).handle());

  return true;
}

void App::destroyBase() {
  m_context.device().waitIdle();

  m_imguiLayer.destroy();
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
    LOGD("poll")

    // Poll glfw events
    m_window.pollEvents();

    LOGD("begin")
    // Wait fence, begin cmd, acquire image
    if (!m_frameManager.beginFrame(f, outputView)) {
      destroy();
      return;
    }
    auto extent = m_context.swapchain().details().extent;

    LOGD("onRender")
    // Record cmd
    onRender(f->cmdBuffer.handle(), f->frameNumber);

    LOGD("imgui begin")
    // Draw gui
    m_imguiLayer.beginFrame();
    renderGUI();
    m_imguiLayer.endFrame(f->cmdBuffer.handle(), outputView, extent);

    LOGD("end")
    // End cmd, Queue submit cmd, present swapchain img, end frame
    if (!m_frameManager.endFrame()) {
      destroy();
      return;
    }
  }
}

void App::renderGUI() {
  ImGui::Begin("Viewport");

  ImVec2 avail = ImGui::GetContentRegionAvail();

  ImGui::Image(m_viewportSet, avail);

  ImGui::End();
}

} // namespace vvhl