#pragma once

#include "vvhl/Vulkan/Memory/SyncState.hpp"
#include <vvhl/vvhl.hpp>

namespace vvhl {
class Buffer;
using BufferHandle = Handle<Buffer>;

struct BufferCreateDescription {
  VkDeviceSize size = 0;

  VkBufferUsageFlags usage{};

  VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO;

  VmaAllocationCreateFlags allocationFlags = 0;
};

struct BufferSyncState : public SyncState {};

struct BufferDescription {
  VkDeviceSize size = 0;

  BufferSyncState syncState = {};

  VkBufferUsageFlags usage{};

  VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO;

  VmaAllocationCreateFlags allocationFlags = 0;
};

class Buffer {
public:
  Buffer() = default;
  ~Buffer() = default;

  Buffer(const Buffer &) = delete;
  Buffer &operator=(const Buffer &) = delete;

  Buffer(Buffer &&other) noexcept;
  Buffer &operator=(Buffer &&other) noexcept;

  bool create(VmaAllocator allocator, const BufferCreateDescription &desc);

  void destroy();

public:
  void *map();
  void unmap();
  bool isMapped() const { return m_mappedPtr != nullptr; }

public:
  VkBuffer handle() const { return m_buffer; }
  VkDeviceSize size() const { return m_desc.size; }
  BufferDescription description() const { return m_desc; }
  BufferSyncState syncState() const { return m_desc.syncState; }

  void setSyncState(BufferSyncState state) {
    m_desc.syncState.access = state.access;
  }

private:
  VmaAllocator m_allocator = VK_NULL_HANDLE;

  VkBuffer m_buffer = VK_NULL_HANDLE;

  VmaAllocation m_allocation = nullptr;

  VmaAllocationInfo m_allocationInfo{};

  BufferDescription m_desc;

  void *m_mappedPtr = nullptr;
};
} // namespace vvhl