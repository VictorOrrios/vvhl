#pragma once

#include "CommandPool.hpp"
#include <vvhl/vvhl.hpp>

namespace vvhl {

class CommandPool;

class CommandBuffer {
public:
  CommandBuffer() = default;
  CommandBuffer(VkCommandBuffer buffer) : m_cmdBuffer(buffer) {};
  ~CommandBuffer() = default;

  CommandBuffer(const CommandBuffer &) = delete;
  CommandBuffer &operator=(const CommandBuffer &) = delete;

  CommandBuffer(CommandBuffer &&other) noexcept
      : m_cmdBuffer(other.m_cmdBuffer) {
    other.m_cmdBuffer = VK_NULL_HANDLE;
  }

  bool begin(VkCommandBufferUsageFlags flags = 0,
             const VkCommandBufferInheritanceInfo *inheritanceInfo = nullptr);

  bool end();

  bool reset(VkCommandBufferResetFlags flags = 0);

public:
  VkCommandBuffer handle() const { return m_cmdBuffer; }

  bool valid() const noexcept { return m_cmdBuffer != VK_NULL_HANDLE; };

  explicit operator bool() const noexcept { return valid(); };

private:
  VkCommandBuffer m_cmdBuffer = VK_NULL_HANDLE;
};

} // namespace vvhl
