#pragma once

#include <vvhl/vvhl.hpp>

namespace vvhl {

struct BufferDescription {
  VkDeviceSize size = 0;

  VkBufferUsageFlags usage{};

  VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO;

  VmaAllocationCreateFlags allocationFlags = 0;
};

class Buffer {
public:
  Buffer() = default;
  ~Buffer() { destroy(); };

  Buffer(const Buffer &) = delete;
  Buffer &operator=(const Buffer &) = delete;

  Buffer(Buffer &&other) noexcept;
  Buffer &operator=(Buffer &&other) noexcept;

  bool create(VmaAllocator allocator, const BufferDescription &desc);

  void destroy();

public:
  void *map();
  void unmap();

public:
  VkBuffer handle() const { return m_buffer; }
  VkDeviceSize size() const { return m_desc.size; }
  BufferDescription description() const { return m_desc; }

  bool isMapped() const { return m_mappedPtr != nullptr; }

private:
  VmaAllocator m_allocator = VK_NULL_HANDLE;

  VkBuffer m_buffer = VK_NULL_HANDLE;

  VmaAllocation m_allocation = nullptr;

  VmaAllocationInfo m_allocationInfo{};

  BufferDescription m_desc;

  void *m_mappedPtr = nullptr;
};
} // namespace vvhl