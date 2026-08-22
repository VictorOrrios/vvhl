#pragma once

#include <vvhl/Vulkan/Context/Device.hpp>

namespace vvhl {
class Semaphore {
public:
  Semaphore() = default;
  ~Semaphore() = default;

  Semaphore(const Semaphore &) = delete;
  Semaphore &operator=(const Semaphore &) = delete;

  Semaphore(Semaphore &&other) noexcept
      : m_device(other.m_device), m_semaphore(other.m_semaphore) {
    other.m_device = nullptr;
    other.m_semaphore = VK_NULL_HANDLE;
  }

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