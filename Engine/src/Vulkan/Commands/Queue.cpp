#include "Queue.hpp"
#include <vulkan/vulkan_core.h>

namespace vvhl {

bool Queue::initialize(VkQueue queue, uint32_t familyIndex) {
  m_queue = queue;
  m_familyIndex = familyIndex;
  return true;
}

bool Queue::submit(const VkSubmitInfo &submitInfo, VkFence fence) const {
  if (vkQueueSubmit(m_queue, 1, &submitInfo, fence) != VK_SUCCESS) {

    LOGE("Failed to submit queue")
    return false;
  }
  return true;
}

void Queue::waitIdle() const { vkQueueWaitIdle(m_queue); }

} // namespace vvhl
