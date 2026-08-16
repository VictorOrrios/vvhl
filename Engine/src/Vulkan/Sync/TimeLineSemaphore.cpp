
#include <vvhl/Vulkan/Sync/TimeLineSemaphore.hpp>

namespace vvhl {

bool TimeLineSemaphore::initialize(Device &device, uint64_t initialValue) {
  VkSemaphoreTypeCreateInfo timelineCreateInfo{};
  timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
  timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
  timelineCreateInfo.initialValue = initialValue;

  VkSemaphoreCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  createInfo.pNext = &timelineCreateInfo;
  createInfo.flags = 0;

  if (vkCreateSemaphore(device.handle(), &createInfo, nullptr,
                        &m_timelineSemaphore) != VK_SUCCESS) {
    LOGE("Failed to create time line semaphore")
    m_timelineSemaphore = VK_NULL_HANDLE;
    return false;
  }

  m_device = &device;
  return true;
}

void TimeLineSemaphore::destroy() {
  if (valid())
    vkDestroySemaphore(m_device->handle(), m_timelineSemaphore, nullptr);
  m_timelineSemaphore = VK_NULL_HANDLE;
  m_device = nullptr;
}

VkResult TimeLineSemaphore::wait(uint64_t value, uint64_t timeout) const {
  ASSERT(valid())

  VkSemaphoreWaitInfo waitInfo{};
  waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;

  waitInfo.flags = 0;
  waitInfo.semaphoreCount = 1;
  waitInfo.pSemaphores = &m_timelineSemaphore;
  waitInfo.pValues = &value;

  return vkWaitSemaphores(m_device->handle(), &waitInfo, timeout);
}

VkResult TimeLineSemaphore::signal(uint64_t value) {
  ASSERT(valid())

  VkSemaphoreSignalInfo signalInfo{};
  signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
  signalInfo.semaphore = m_timelineSemaphore;
  signalInfo.value = value;

  return vkSignalSemaphore(m_device->handle(), &signalInfo);
}

uint64_t TimeLineSemaphore::value() const {
  ASSERT(valid())

  uint64_t value = 0;

  if (vkGetSemaphoreCounterValue(m_device->handle(), m_timelineSemaphore,
                                 &value) != VK_SUCCESS) {
    LOGE("Failed getting timeline sempahore value")
  }

  return value;
}

} // namespace vvhl
