
#include <vvhl/Vulkan/Sync/Semaphore.hpp>

namespace vvhl {

bool Semaphore::initialize(Device &device, bool signaled) {
  VkSemaphoreCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  createInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

  if (vkCreateSemaphore(device.handle(), &createInfo, nullptr, &m_semaphore) !=
      VK_SUCCESS) {
    LOGE("Failed to create semaphore")
    m_semaphore = VK_NULL_HANDLE;
    return false;
  }

  m_device = &device;
  return true;
}

void Semaphore::destroy() {
  if (valid())
    vkDestroySemaphore(m_device->handle(), m_semaphore, nullptr);
  m_semaphore = VK_NULL_HANDLE;
  m_device = nullptr;
}

} // namespace vvhl