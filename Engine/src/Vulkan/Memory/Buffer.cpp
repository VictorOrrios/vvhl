#include "Buffer.hpp"

namespace vvhl {

bool Buffer::create(VmaAllocator allocator, const BufferDescription &desc) {
  m_allocator = allocator;
  m_size = desc.size;

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = desc.size;
  bufferInfo.usage = desc.usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = desc.memoryUsage;
  allocInfo.flags = desc.allocationFlags;

  VkResult result =
      vmaCreateBuffer(m_allocator, &bufferInfo, &allocInfo, &m_buffer,
                      &m_allocation, &m_allocationInfo);

  if (result != VK_SUCCESS) {
    LOGE("Failed to create buffer");
    return false;
  }

  if (m_allocationInfo.pMappedData)
    m_mappedPtr = m_allocationInfo.pMappedData;

  return true;
}

void *Buffer::map() {
  if (m_mappedPtr)
    return m_mappedPtr;

  VkResult result = vmaMapMemory(m_allocator, m_allocation, &m_mappedPtr);

  if (result != VK_SUCCESS) {
    LOGE("Failed to map buffer");
    return nullptr;
  }

  return m_mappedPtr;
}

void Buffer::unmap() {
  if (!m_mappedPtr)
    return;

  vmaUnmapMemory(m_allocator, m_allocation);
  m_mappedPtr = nullptr;
}

void Buffer::destroy() {
  if (m_buffer == VK_NULL_HANDLE || m_allocator == VK_NULL_HANDLE)
    return;

  vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);

  m_buffer = VK_NULL_HANDLE;
  m_allocation = nullptr;
  m_allocationInfo = {};
  m_mappedPtr = nullptr;
  m_size = 0;
  m_allocator = VK_NULL_HANDLE;
}

} // namespace vvhl