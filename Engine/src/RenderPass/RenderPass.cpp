
#include <vvhl/RenderPass/RenderPass.hpp>

namespace vvhl {

bool RenderPass::initializeBase(App &app) {
  m_context = &app.context();
  m_resourceManager = &app.resourceManager();
  m_eventDispatcher = &app.eventDispatcher();
  m_barriers.initialize(app.resourceManager());
  m_gbuffers = &app.gbuffers();

  m_eventDispatcher->subscribe<ViewportResizeEvent>(
      [this](const ViewportResizeEvent &e) { this->onViewportResize(e); });

  return true;
}

void RenderPass::destroyBase() {
  m_context = nullptr;
  m_resourceManager = nullptr;
  m_eventDispatcher = nullptr;
  m_gbuffers = nullptr;
  m_descPool.destroy();
  m_renderer.invalidateRenderingInfo();
  m_barriers.destroy();
}

void RenderPass::execute(VkCommandBuffer cmd, uint32_t currentFrame){
  onRender(cmd,currentFrame);
}

} // namespace vvhl