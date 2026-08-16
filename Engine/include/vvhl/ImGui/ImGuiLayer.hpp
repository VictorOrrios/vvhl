#pragma once

#include <vvhl/Vulkan/Descriptors/DescriptorSet.hpp>
#include <vvhl/Vulkan/Renderer/DynamicRenderer.hpp>
#include <vvhl/Vulkan/Descriptors/DescriptorPool.hpp>
#include <vvhl/Vulkan/Commands/CommandSystem.hpp>
#include <vvhl/Vulkan/Context/VulkanContext.hpp>
#include <vvhl/vvhl.hpp>

namespace vvhl {

class ImGuiLayer {
public:
  ImGuiLayer() = default;
  ~ImGuiLayer() { destroy(); }

  ImGuiLayer(const ImGuiLayer &) = delete;
  ImGuiLayer &operator=(const ImGuiLayer &) = delete;

  bool initialize(VulkanContext &context, Window &window);
  void destroy();

  void beginFrame();

  void endFrame(VkCommandBuffer cmd, VkImageView targetView, VkExtent2D extent);

  VkExtent2D getViewportExtent() const;

  VkDescriptorSet createViewportTextureId(VkImageView image, VkSampler sampler);

private:
  bool createDescriptorPool();
  bool createFontTexture(CommandPool &cmdPool);

  VkCommandBuffer beginSingleTimeCommands(CommandPool &cmdPool);
  void endSingleTimeCommands(CommandPool &cmdPool, VkCommandBuffer cmd);

private:
  VkDevice m_device = VK_NULL_HANDLE;
  VulkanContext *m_context = nullptr;
  Window *m_window = nullptr;

  DescriptorPool m_descriptorPool;
  DynamicRenderer m_renderer;
};

} // namespace vvhl