
#include "RenderPass.hpp"

namespace vvhl {

bool RenderPass::initializeBase(App &app) {
  m_context = &app.context();
  m_resourceManager = &app.resourceManager();

  return true;
}

void RenderPass::destroyBase() {
  m_context = nullptr;
  m_resourceManager = nullptr;
  m_descPool.destroy();
  m_renderer.invalidateRenderingInfo();
}

void RenderPass::execute(VkCommandBuffer cmd, uint32_t currentFrame){
  m_renderer.begin(cmd);
  onRender(cmd,currentFrame);
  m_renderer.end(cmd);
}

} // namespace vvhl