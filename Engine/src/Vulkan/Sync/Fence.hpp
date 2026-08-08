#pragma once

#include <vvhl/vvhl.hpp>
#include <Vulkan/Context/Device.hpp>

namespace vvhl {

class Fence {
public:
  Fence() = default;
  ~Fence() { destroy(); };

  Fence(const Fence &) = delete;
  Fence &operator=(const Fence &) = delete;

  bool initialize(Device &device, bool signaled = false);
  void destroy();

  VkResult wait(uint64_t timeout = UINT64_MAX) const;
  VkResult reset();

  bool isSignaled() const;

public:
  VkFence handle() const noexcept {return m_fence; }
  bool valid() const noexcept { return m_fence != VK_NULL_HANDLE; }

private:
  Device *m_device = nullptr;
  VkFence m_fence = VK_NULL_HANDLE;
};

} // namespace vvhl