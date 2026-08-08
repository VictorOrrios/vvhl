
#include "Fence.hpp"
#include <vulkan/vulkan_core.h>

namespace vvhl {

bool Fence::initialize(Device &device, bool signaled) {
  VkFenceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  createInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

  if (vkCreateFence(device.handle(), &createInfo, nullptr, &m_fence) !=
      VK_SUCCESS) {
    LOGE("Failed to create fence")
    m_fence = VK_NULL_HANDLE;
    return false;
  }

  m_device = &device;
  return true;
}

void Fence::destroy() {
  if (valid())
    vkDestroyFence(m_device->handle(), m_fence, nullptr);
  m_fence = VK_NULL_HANDLE;
  m_device = nullptr;
}

VkResult Fence::wait(uint64_t timeout) const {
  ASSERT(valid())
  return vkWaitForFences(m_device->handle(), 1, &m_fence, VK_TRUE, timeout);
}

VkResult Fence::reset() {
  ASSERT(valid())
  return vkResetFences(m_device->handle(), 1, &m_fence);
}

bool Fence::isSignaled() const {
  return vkGetFenceStatus(m_device->handle(), m_fence) == VK_SUCCESS;
}

} // namespace vvhl
