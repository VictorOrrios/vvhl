#include <vvhl/Vulkan/Renderer/DynamicRenderer.hpp>
#include <vvhl/Core/Logger.hpp>

namespace vvhl
{

  void DynamicRenderer::begin(VkCommandBuffer cmd)
  {
    if (!m_cached)
    {
      LOGE("Rendering info empty, use updateRenderingInfo")
      return;
    }
    vkCmdBeginRendering(cmd, &m_cache.renderingInfo);
  }

  void DynamicRenderer::end(VkCommandBuffer cmd) { vkCmdEndRendering(cmd); }

  void DynamicRenderer::updateRenderingInfo(const RenderingConfig &config)
  {
    m_cache.config = config;
    m_cache.hasDepth = config.depthAttachment.has_value();

    // Color attachments
    m_cache.colorAttachments.clear();
    m_cache.colorAttachments.reserve(config.colorAttachments.size());

    for (const auto &attachment : config.colorAttachments)
    {
      m_cache.colorAttachments.push_back(createColorAttachmentInfo(attachment));
    }

    // Depth attachment
    if (m_cache.hasDepth)
    {
      m_cache.depthAttachment =
          createDepthAttachmentInfo(config.depthAttachment.value());
    }

    // Rendering info
    m_cache.renderingInfo = {};
    m_cache.renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    m_cache.renderingInfo.renderArea = config.renderArea;
    m_cache.renderingInfo.layerCount = config.layerCount;
    m_cache.renderingInfo.viewMask = config.viewMask;
    m_cache.renderingInfo.colorAttachmentCount =
        static_cast<uint32_t>(m_cache.colorAttachments.size());
    m_cache.renderingInfo.pColorAttachments = m_cache.colorAttachments.data();

    if (m_cache.hasDepth)
    {
      m_cache.renderingInfo.pDepthAttachment = &m_cache.depthAttachment;
      m_cache.renderingInfo.pStencilAttachment = &m_cache.depthAttachment;
    }

    m_cached = true;
  }

  VkRenderingAttachmentInfo
  DynamicRenderer::createColorAttachmentInfo(const ColorAttachment &attachment)
  {

    VkRenderingAttachmentInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    info.imageView = attachment.view;
    info.imageLayout = attachment.layout;
    info.loadOp = attachment.loadOp;
    info.storeOp = attachment.storeOp;

    if (attachment.loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR)
    {
      info.clearValue = attachment.clearValue;
    }

    // MSAA resolve
    if (attachment.resolveView != VK_NULL_HANDLE)
    {
      info.resolveMode = attachment.resolveMode;
      info.resolveImageView = attachment.resolveView;
      info.resolveImageLayout = attachment.resolveLayout;
    }

    return info;
  }

  VkRenderingAttachmentInfo
  DynamicRenderer::createDepthAttachmentInfo(const DepthAttachment &attachment)
  {

    VkRenderingAttachmentInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    info.imageView = attachment.view;
    info.imageLayout = attachment.layout;
    info.loadOp = attachment.loadOp;
    info.storeOp = attachment.storeOp;

    if (attachment.loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR)
    {
      info.clearValue.depthStencil.depth = attachment.depthClearValue;
      info.clearValue.depthStencil.stencil = attachment.stencilClearValue;
    }

    return info;
  }

  void DynamicRenderer::setViewportAndScissor(VkCommandBuffer cmd)
  {
    setViewportAndScissor(cmd, m_cache.renderingInfo.renderArea);
  }

  void DynamicRenderer::setViewportAndScissor(VkCommandBuffer cmd, const VkRect2D &area)
  {
    VkViewport viewport{};
    viewport.x = static_cast<float>(area.offset.x);
    viewport.y = static_cast<float>(area.offset.y);
    viewport.width = static_cast<float>(area.extent.width);
    viewport.height = static_cast<float>(area.extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &area);
  }

  void DynamicRenderer::setViewportAndScissor(VkCommandBuffer cmd, const VkViewport &viewport, const VkRect2D &scissor)
  {
    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &scissor);
  }

} // namespace vvhl