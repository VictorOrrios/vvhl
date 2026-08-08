#pragma once

#include "Vulkan/Context/Device.hpp"
#include <vulkan/vulkan_core.h>
#include <vvhl/vvhl.hpp>

namespace vvhl {
class Semaphore {
public:
  Semaphore() = default;
  ~Semaphore() { destroy(); };

  Semaphore(const Semaphore &) = delete;
  Semaphore &operator=(const Semaphore &) = delete;

  bool initialize(Device &device, bool signaled = false);
  void destroy();

public:
  VkSemaphore handle() const noexcept {return m_semaphore; }
  bool valid() const noexcept { return m_semaphore != VK_NULL_HANDLE; }

private:
  Device *m_device = nullptr;
  VkSemaphore m_semaphore = VK_NULL_HANDLE;
};

} // namespace vvhl