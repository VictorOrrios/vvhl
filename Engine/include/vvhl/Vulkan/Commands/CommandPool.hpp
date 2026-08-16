#pragma once

#include "CommandBuffer.hpp"
#include <vvhl/Vulkan/Context/Device.hpp>

namespace vvhl {

class CommandBuffer;

class CommandPool {
public:
  CommandPool() = default;
  ~CommandPool() { destroy(); };

  CommandPool(const CommandPool &) = delete;
  CommandPool &operator=(const CommandPool &) = delete;

  bool initialize(Device &device, uint32_t queueFamilyIndex,
                  VkCommandPoolCreateFlags flags =
                      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  void destroy();

  void reset(VkCommandPoolResetFlags flags = 0);

  CommandBuffer
  allocate(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

public:
  VkCommandPool handle() const { return m_pool; }
  uint32_t queueFamilyIndex() const { return m_queueFamilyIndex; }

private:
  VkCommandPool m_pool = VK_NULL_HANDLE;
  Device *m_device = nullptr;
  uint32_t m_queueFamilyIndex = 0;
};

} // namespace vvhl
