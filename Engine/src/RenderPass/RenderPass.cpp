
#include <vvhl/RenderPass/RenderPass.hpp>

namespace vvhl {

bool RenderPass::initializeBase(App &app) {
  m_context = &app.context();
  m_resourceManager = &app.resourceManager();
  m_barriers.initialize(app.resourceManager());
  return true;
}

void RenderPass::destroyBase() {
  m_context = nullptr;
  m_resourceManager = nullptr;
  m_descPool.destroy();
  m_renderer.invalidateRenderingInfo();
  m_barriers.destroy();
}

void RenderPass::execute(VkCommandBuffer cmd, uint32_t currentFrame){
  onRender(cmd,currentFrame);
}

} // namespace vvhl