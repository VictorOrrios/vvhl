
#include <vvhl/Vulkan/Commands/CommandBuffer.hpp>

namespace vvhl {

bool CommandBuffer::begin(
    VkCommandBufferUsageFlags flags,
    const VkCommandBufferInheritanceInfo *inheritanceInfo) {

  VkCommandBufferBeginInfo beginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = flags,
      .pInheritanceInfo = inheritanceInfo};

  VkResult result = vkBeginCommandBuffer(m_cmdBuffer, &beginInfo);

  if (result != VK_SUCCESS) {
    LOGE("Failed to begin command buffer");
    return false;
  }

  return true;
}

bool CommandBuffer::end() {
  VkResult result = vkEndCommandBuffer(m_cmdBuffer);

  if (result != VK_SUCCESS) {
    LOGE("Failed to end command buffer");
    return false;
  }

  return true;
}

bool CommandBuffer::reset(VkCommandBufferResetFlags flags) {
  VkResult result = vkResetCommandBuffer(m_cmdBuffer, flags);

  if (result != VK_SUCCESS) {
    LOGE("Failed to reset command buffer");
    return false;
  }

  return true;
}

} // namespace vvhl