#include "vvhl/Vulkan/Commands/CommandBuffer.hpp"
#include <vvhl/Vulkan/Commands/Queue.hpp>

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

bool Queue::submit(const VkCommandBuffer cmd, VkFence fence) const {
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.pNext = nullptr;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &cmd;
  return submit(submitInfo,fence);
}

void Queue::waitIdle() const { vkQueueWaitIdle(m_queue); }

bool Queue::submitWait(const VkCommandBuffer cmd){
    if(!submit(cmd))
        return false;
    waitIdle();
    return true;
}

} // namespace vvhl
