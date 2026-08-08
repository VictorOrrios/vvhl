#pragma once

#include <vulkan/vulkan_core.h>
#include <vvhl/vvhl.hpp>

namespace vvhl {

class Queue {
public:
  Queue() = default;
  ~Queue() = default;

  Queue(const Queue &) = delete;
  Queue &operator=(const Queue &) = delete;

  bool initialize(VkQueue queue, uint32_t familyIndex);

  bool submit(const VkSubmitInfo &submitInfo, VkFence fence = VK_NULL_HANDLE) const;  
  void waitIdle() const;

public:
  VkQueue handle() const { return m_queue; }
  uint32_t familyIndex() const { return m_familyIndex; }

  bool valid() const noexcept { return m_queue != VK_NULL_HANDLE; };

  explicit operator bool() const noexcept { return valid(); };

private:
  VkQueue m_queue = VK_NULL_HANDLE;
  uint32_t m_familyIndex = 0;
};

} // namespace vvhl
