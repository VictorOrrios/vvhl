#pragma once

#include "Vulkan/Descriptors/DescriptorPool.hpp"
#include "Vulkan/Renderer/DynamicRenderer.hpp"
#include "vvhl/Core/App.hpp"

namespace vvhl {

class RenderPass {
public:
  RenderPass() = default;
  ~RenderPass() { destroy(); };

  RenderPass(const RenderPass &) = delete;
  RenderPass &operator=(const RenderPass &) = delete;

  virtual void destroy();  

  void execute(VkCommandBuffer cmdBuff, uint32_t currentFrame);

public:
  VulkanContext &context() { return *m_context; }
  ResourceManager &resourceManager() { return *m_resourceManager; }
  DescriptorPool &descriptorPool() { return m_descPool; }

protected:
  bool initializeBase(App &app);
  void destroyBase();

  virtual void onRender(VkCommandBuffer cmdBuff, uint32_t currentFrame);

protected:
  VulkanContext *m_context = nullptr;
  ResourceManager *m_resourceManager = nullptr;
  DescriptorPool m_descPool;
  DynamicRenderer m_renderer;
};

} // namespace vvhl