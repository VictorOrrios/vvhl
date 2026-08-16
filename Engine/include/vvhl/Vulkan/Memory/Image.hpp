#pragma once

#include <vvhl/Vulkan/Context/VulkanContext.hpp>

namespace vvhl {

class VulkanContext;

struct ImageDescription {
  VkImageType type = VK_IMAGE_TYPE_2D;

  VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

  uint32_t width = 1;
  uint32_t height = 1;
  uint32_t depth = 1;

  uint32_t mipLevels = 1;
  uint32_t arrayLayers = 1;

  VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

  VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;

  VkImageCreateFlags createFlags = 0;

  VkImageUsageFlags usage = 0;
  
  VkImageViewType viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;

  VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
  VmaAllocationCreateFlags allocationFlags = 0;
};


class Image {
public:
  Image() = default;
  ~Image() { destroy(); };

  Image(const Image &) = delete;
  Image &operator=(const Image &) = delete;

  Image(Image &&other) noexcept;
  Image &operator=(Image &&other) noexcept;

  bool create(VulkanContext &context, const ImageDescription &desc);

  void destroy();

public:
  VkImage handle() const { return m_image; }

  VkImageView view() const { return m_view; }

  VkExtent3D extent() const { return {m_desc.width, m_desc.height, m_desc.depth}; }

  VkFormat format() const { return m_desc.format; }

  uint32_t width() const { return m_desc.width; }

  uint32_t height() const { return m_desc.height; }

  uint32_t depth() const { return m_desc.depth; }

  uint32_t mipLevels() const { return m_desc.mipLevels; }

  ImageDescription description() const { return m_desc; }

private:
  VkImageAspectFlags getAspectMask(VkFormat format);
private:
  VulkanContext *m_context = nullptr;

  VkImage m_image = VK_NULL_HANDLE;
  VkImageView m_view = VK_NULL_HANDLE;

  VmaAllocation m_allocation = nullptr;
  VmaAllocationInfo m_allocationInfo{};

  ImageDescription m_desc{};
};

} // namespace vvhl