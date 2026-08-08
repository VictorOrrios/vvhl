
#include "CommandPool.hpp"
#include "CommandBuffer.hpp"

namespace vvhl {

bool CommandPool::initialize(Device &device, uint32_t queueFamilyIndex,
                             VkCommandPoolCreateFlags flags) {

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = flags;
  poolInfo.queueFamilyIndex = queueFamilyIndex;

  if (vkCreateCommandPool(device.handle(), &poolInfo, nullptr, &m_pool) !=
      VK_SUCCESS) {
    m_pool = VK_NULL_HANDLE;
    LOGE("Failed to create command pool")
    return false;
  }

  m_device = &device;
  m_queueFamilyIndex = queueFamilyIndex;

  return true;
}

void CommandPool::destroy() {
  if (m_pool == VK_NULL_HANDLE)
    return;

  vkDestroyCommandPool(m_device->handle(), m_pool, nullptr);

  m_pool = VK_NULL_HANDLE;
  m_device = nullptr;
  m_queueFamilyIndex = 0;
}

CommandBuffer CommandPool::allocate(VkCommandBufferLevel level) {
  VkCommandBufferAllocateInfo allocateInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = m_pool,
      .level = level,
      .commandBufferCount = 1};

  VkCommandBuffer buffer = VK_NULL_HANDLE;

  if (vkAllocateCommandBuffers(m_device->handle(), &allocateInfo, &buffer) !=
      VK_SUCCESS) {
    LOGE("Failed to allocate command buffer");
    return CommandBuffer();
  }

  return CommandBuffer(buffer);
}

void CommandPool::reset(VkCommandPoolResetFlags flags) {
  vkResetCommandPool(m_device->handle(), m_pool, flags);
}

} // namespace vvhl