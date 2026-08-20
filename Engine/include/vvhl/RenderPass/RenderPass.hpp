#pragma once

#include "vvhl/Core/App.hpp"
#include "vvhl/Vulkan/Sync/Barriermanager.hpp"
#include <vvhl/Vulkan/Descriptors/DescriptorPool.hpp>
#include <vvhl/Vulkan/Renderer/DynamicRenderer.hpp>

namespace vvhl {

class RenderPass {
public:
  RenderPass() = default;
  ~RenderPass() = default;

  RenderPass(const RenderPass &) = delete;
  RenderPass &operator=(const RenderPass &) = delete;

  virtual void destroy() { destroyBase(); };

  void execute(VkCommandBuffer cmd, uint32_t currentFrame);

public:
  VulkanContext &context() { return *m_context; }
  ResourceManager &resourceManager() { return *m_resourceManager; }
  DescriptorPool &descriptorPool() { return m_descPool; }

protected:
  bool initializeBase(App &app);
  void destroyBase();

  virtual void onRender(VkCommandBuffer, uint32_t) {};

protected:
  VulkanContext *m_context = nullptr;
  ResourceManager *m_resourceManager = nullptr;
  DescriptorPool m_descPool;
  DynamicRenderer m_renderer;
  BarrierManager m_barriers;
};

} // namespace vvhl