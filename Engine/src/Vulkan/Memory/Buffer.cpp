#include <vvhl/Vulkan/Memory/Buffer.hpp>

namespace vvhl {

Buffer::Buffer(Buffer &&other) noexcept
    : m_allocator(other.m_allocator), m_buffer(other.m_buffer),
      m_allocation(other.m_allocation),
      m_allocationInfo(other.m_allocationInfo), m_desc(other.m_desc),
      m_mappedPtr(other.m_mappedPtr) {

  other.m_allocator = VK_NULL_HANDLE;
  other.m_buffer = VK_NULL_HANDLE;
  other.m_allocation = nullptr;
  other.m_allocationInfo = {};
  other.m_desc = {};
  other.m_mappedPtr = nullptr;
}

bool Buffer::create(VmaAllocator allocator, const BufferDescription &desc) {
  m_allocator = allocator;
  m_desc = desc;

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
  m_desc = {};
  m_allocator = VK_NULL_HANDLE;
}

} // namespace vvhl