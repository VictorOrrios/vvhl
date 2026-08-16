#pragma once

#include <vvhl/Vulkan/Context/Device.hpp>

namespace vvhl {
class TimeLineSemaphore {
public:
  TimeLineSemaphore() = default;
  ~TimeLineSemaphore() { destroy(); };

  TimeLineSemaphore(const TimeLineSemaphore &) = delete;
  TimeLineSemaphore &operator=(const TimeLineSemaphore &) = delete;

  bool initialize(Device &device, uint64_t initialValue = 0);
  void destroy();

  VkResult wait(uint64_t value, uint64_t timeout = UINT64_MAX) const;

  VkResult signal(uint64_t value);

  uint64_t value() const;

public:
  VkSemaphore handle() const noexcept { return m_timelineSemaphore; }
  bool valid() const noexcept { return m_timelineSemaphore != VK_NULL_HANDLE; }

private:
  Device *m_device = nullptr;
  VkSemaphore m_timelineSemaphore = VK_NULL_HANDLE;
};

} // namespace vvhl