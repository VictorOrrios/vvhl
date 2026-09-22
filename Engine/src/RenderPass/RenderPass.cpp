
#include "vvhl/Core/EngineContext.hpp"
#include <vvhl/RenderPass/RenderPass.hpp>

namespace vvhl {

bool RenderPass::initializeBase(EngineContext ctx) {
  m_ctx = ctx;
  m_barriers.initialize(*m_ctx.resourceManager);

  m_ctx.eventDispatcher->subscribe<ViewportResizeEvent>(
      [this](const ViewportResizeEvent &e) { this->onViewportResize(e); });

  return true;
}

void RenderPass::destroyBase() {
  m_ctx = {};
  m_descPool.destroy();
  m_renderer.invalidateRenderingInfo();
  m_barriers.destroy();
}

void RenderPass::execute(VkCommandBuffer cmd, uint32_t currentFrame) {
  onRender(cmd, currentFrame);
}

} // namespace vvhl
