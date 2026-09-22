#pragma once

#include "vvhl/Core/App.hpp"
#include "vvhl/Core/EngineContext.hpp"
#include "vvhl/Vulkan/Sync/Barriermanager.hpp"
#include <string_view>
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
  EngineContext &context() { return m_ctx; }
  VulkanContext &vkContext() { return *m_ctx.vkContext; }
  ResourceManager &resourceManager() { return *m_ctx.resourceManager; }
  DescriptorPool &descriptorPool() { return m_descPool; }
  std::string_view name() { return m_name; }

protected:
  bool initializeBase(EngineContext ctx);
  void destroyBase();

  virtual void onRender(VkCommandBuffer, uint32_t) {};
  virtual void onRenderGUI() {};
  virtual void onViewportResize(const ViewportResizeEvent &) {};

protected:
  EngineContext m_ctx;
  DescriptorPool m_descPool;
  DynamicRenderer m_renderer;
  BarrierManager m_barriers;
  std::string m_name = "My renderpass";
};

} // namespace vvhl
