#include "imgui_internal.h"
#include "vvhl/Events/AppEvents.hpp"
#include "vvhl/ImGui/ImGuiLayer.hpp"
#include <vvhl/Core/App.hpp>
#include <vvhl/Core/GLFWContext.hpp>
#include <vvhl/Core/Window.hpp>
#include <vvhl/Events/WindowEvents.hpp>

namespace vvhl {

bool App::initialize(AppConfig config) {
  return initializeBase(config) && onAttach();
}

void App::destroy(){
    onDestroy();
    destroyBase();
}

bool App::initializeBase(const AppConfig &config) {
  Logger::init();

  GLFWContext::init();
  LOGI("Initialized: GLFW Context")

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

  m_eventDispatcher.subscribe<WindowCloseEvent>(
      [this](const WindowCloseEvent &) { m_shouldClose = true; });

  m_eventDispatcher.subscribe<FramebufferResizeEvent>(
      [this](const FramebufferResizeEvent &e) {
        this->onFrameBufferResize(e);
      });

  m_eventDispatcher.subscribe<ViewportResizeEvent>(
      [this](const ViewportResizeEvent &e) { this->onViewportResize(e); });

  return true;
}

bool App::createViewport() {
  VkExtent2D viewportSize = VkExtent2D(100, 100);

  // Create image and sampler with the resource manager
  m_viewport = m_resourceManager.createImage({
      .format = VK_FORMAT_B8G8R8A8_UNORM,
      .width = viewportSize.width,
      .height = viewportSize.height,
      .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | // Graphics pipelines
               VK_IMAGE_USAGE_STORAGE_BIT |          // Compute pipelines
               VK_IMAGE_USAGE_SAMPLED_BIT |          // ImGui sampling
               VK_IMAGE_USAGE_TRANSFER_DST_BIT |     // To blit/copy if necesary
               VK_IMAGE_USAGE_TRANSFER_SRC_BIT, // To read back for debugging or
                                                // exports
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
  });

  m_viewportSampler = m_resourceManager.createSampler(
      {.magFilter = VK_FILTER_LINEAR,
       .minFilter = VK_FILTER_LINEAR,

       .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
       .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
       .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,

       .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
       .maxLod = 0.0f});

  // Register image in imgui
  m_viewportSet = m_imguiLayer.createViewportTextureId(
      m_resourceManager.image(m_viewport).view(),
      m_resourceManager.sampler(m_viewportSampler).handle());

  // Register image in gbuffers
  if (!m_gbuffers.initialize(m_resourceManager)) {
    return false;
  }

  // GBufferID 0 is reserved for the viewport
  m_gbuffers.add(0, m_viewport);

  return true;
}

EngineContext App::makeEngineContext() {
  return {
      .eventDispatcher = &m_eventDispatcher,
      .vkContext = &m_context,
      .resourceManager = &m_resourceManager,
      .gbuffers = &m_gbuffers,
      .cmdSystem = &m_cmdSystem,
      .frameManager = &m_frameManager,
  };
}

void App::destroyBase() {
  m_context.device().waitIdle();

  m_gbuffers.destroy();
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
    // Poll glfw events
    m_window.pollEvents();

    // Poll event dispatcher events
    m_eventDispatcher.poll();

    // Wait fence, begin cmd, acquire image, transition layout
    if (!m_frameManager.beginFrame(f, outputView)) {
      destroy();
      return;
    }
    auto swapchain_extent = m_context.swapchain().details().extent;
    // auto framebuffer_extent = m_window.getFramebufferSize();

    // Record cmd
    onRender(f->cmdBuffer.handle(), f->frameNumber);

    // Draw gui
    m_imguiLayer.beginFrame();
    renderGUI();
    m_imguiLayer.endFrame(f->cmdBuffer.handle(), outputView, swapchain_extent);

    // Transition layout, end cmd, Queue submit cmd, present swapchain img, end
    // frame
    if (!m_frameManager.endFrame()) {
      destroy();
      return;
    }
  }
  // Wait idle before closing
  m_context.device().waitIdle();
}

void App::renderGUI() {
  ImGuiID dockspace_id = ImGui::GetID("Dockspace");
  ImGuiViewport *viewport = ImGui::GetMainViewport();

  // Create settings
  if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr) {
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);
    ImGuiID dock_id_left = 0;
    ImGuiID dock_id_main = dockspace_id;
    ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Left, 0.20f,
                                &dock_id_left, &dock_id_main);
    ImGuiID dock_id_left_top = 0;
    ImGuiID dock_id_left_bottom = 0;
    ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Up, 0.50f,
                                &dock_id_left_top, &dock_id_left_bottom);
    ImGui::DockBuilderDockWindow("Viewport", dock_id_main);
    ImGui::DockBuilderDockWindow("Panel A", dock_id_left_top);
    ImGui::DockBuilderDockWindow("Render Graph", dock_id_left_bottom);
    ImGui::DockBuilderFinish(dockspace_id);
  }

  ImGui::DockSpaceOverViewport(dockspace_id, viewport);

  ImGui::Begin("Viewport");

  ImVec2 avail = ImGui::GetContentRegionAvail();

  if (m_viewportSize.x != avail.x || m_viewportSize.y != avail.y) {
    auto scale = ImGui::GetIO().DisplayFramebufferScale;
    m_eventDispatcher.enqueue(
        ViewportResizeEvent(scale.x * avail.x, scale.y * avail.y));
    m_viewportSize = ImVec2(avail.x * scale.x, avail.y * scale.y);
  }

  ImVec2 pos_min = ImGui::GetCursorScreenPos();
  ImVec2 pos_max = ImVec2(pos_min.x + avail.x, pos_min.y + avail.y);
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  draw_list->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerNearest);
  draw_list->AddImage(m_viewportSet, pos_min, pos_max);
  draw_list->AddCallback(ImGui::GetPlatformIO().DrawCallback_ResetRenderState);

  // Using image applies a linear sampling
  // ImGui::Image(m_viewportSet, avail);

  ImGui::End();

  ImGui::Begin("Panel A");
  ImGui::Text("Test of panel A");
  static int counter = 0;
  if (ImGui::Button("Increment")) {
    counter++;
  }
  ImGui::Text("Counter: %d", counter);
  ImGui::End();

  ImGui::Begin("Render Graph");
  onRenderGraphGUI();
  ImGui::End();
}

void App::onFrameBufferResize(const FramebufferResizeEvent &e) {
  // auto extent = m_context.swapchain().details().extent;
  // if(extent.width >= e.Width && extent.height >= e.Height) return;

  m_context.device().waitIdle();

  m_context.swapchain().recreate(e.Width, e.Height);
}

void App::onViewportResize(const ViewportResizeEvent &e) {
  m_context.device().waitIdle();
  m_gbuffers.resize(e.width, e.height);
  // LOGI("Viewport resize event {}x{}",e.width,e.height)

  ImGui_ImplVulkan_RemoveTexture(m_viewportSet);
  m_viewportSet = m_imguiLayer.createViewportTextureId(
      m_resourceManager.image(m_viewport).view(),
      m_resourceManager.sampler(m_viewportSampler).handle());
}

} // namespace vvhl
