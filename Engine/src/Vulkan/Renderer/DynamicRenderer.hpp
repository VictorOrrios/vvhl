#pragma once

namespace vvhl {

struct ColorAttachment {
  VkImageView view = VK_NULL_HANDLE;
  VkImageLayout layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  VkClearValue clearValue = {};
  VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

  // MSAA resolve
  VkImageView resolveView = VK_NULL_HANDLE;
  VkImageLayout resolveLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  VkResolveModeFlagBits resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
};

struct DepthAttachment {
  VkImageView view = VK_NULL_HANDLE;
  VkImageLayout layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  float depthClearValue = 1.0f;
  uint32_t stencilClearValue = 0;
  VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE;
};

struct RenderingConfig {
  std::vector<ColorAttachment> colorAttachments;
  std::optional<DepthAttachment> depthAttachment;
  VkRect2D renderArea = {{0, 0}, {0, 0}};
  uint32_t layerCount = 1;
  uint32_t viewMask = 0;

  // Helpers
  static RenderingConfig singleColor(VkImageView view, VkExtent2D extent) {
    RenderingConfig config;
    ColorAttachment color;
    color.view = view;
    config.colorAttachments.push_back(color);
    config.renderArea.extent = extent;
    return config;
  }

  static RenderingConfig colorDepth(VkImageView colorView,
                                    VkImageView depthView, VkExtent2D extent) {
    RenderingConfig config;
    ColorAttachment color;
    color.view = colorView;
    config.colorAttachments.push_back(color);

    DepthAttachment depth;
    depth.view = depthView;
    config.depthAttachment = depth;

    config.renderArea.extent = extent;
    return config;
  }

};

class DynamicRenderer {
public:
  DynamicRenderer() = default;
  ~DynamicRenderer() = default;

  DynamicRenderer(const DynamicRenderer &) = delete;
  DynamicRenderer &operator=(const DynamicRenderer &) = delete;

  void begin(VkCommandBuffer cmd);
  void end(VkCommandBuffer cmd);

  void updateRenderingInfo(const RenderingConfig &config);
  void invalidateRenderingInfo() { m_cached = false; }

private:
  struct CachedRendering {
    RenderingConfig config;
    VkRenderingInfo renderingInfo = {};
    std::vector<VkRenderingAttachmentInfo> colorAttachments;
    VkRenderingAttachmentInfo depthAttachment = {};
    bool hasDepth = false;
  };

  VkRenderingAttachmentInfo
  createColorAttachmentInfo(const ColorAttachment &attachment);
  VkRenderingAttachmentInfo
  createDepthAttachmentInfo(const DepthAttachment &attachment);

  CachedRendering m_cache;
  bool m_cached = false;
};

} // namespace vvhl