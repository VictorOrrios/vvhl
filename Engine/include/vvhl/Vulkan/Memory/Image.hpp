#pragma once

#include <vvhl/Vulkan/Context/VulkanContext.hpp>
#include "vvhl/Vulkan/Memory/SyncState.hpp"

namespace vvhl {
class Image;
using ImageHandle = Handle<Image>;
  
class VulkanContext;

constexpr VkImageAspectFlags _autoAspectMask =
    VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;

struct ImageCreateDescription {
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

  VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  VkImageViewType viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;

  VkImageAspectFlags aspectMask = _autoAspectMask;

  VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
  VmaAllocationCreateFlags allocationFlags = 0;
};

struct ImageSyncState : public SyncState {
  VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
  uint32_t baseMipLevel = 0;
  uint32_t mipLevelCount = VK_REMAINING_MIP_LEVELS;
  uint32_t baseArrayLayer = 0;
  uint32_t arrayLayerCount = VK_REMAINING_ARRAY_LAYERS;
};

struct ImageDescription {
  VkImageType type = VK_IMAGE_TYPE_2D;

  VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

  uint32_t width = 1;
  uint32_t height = 1;
  uint32_t depth = 1;

  ImageSyncState syncState = {};

  VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

  VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;

  VkImageCreateFlags createFlags = 0;

  VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  VkImageViewType viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;

  VkImageAspectFlags aspectMask = _autoAspectMask;

  VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
  VmaAllocationCreateFlags allocationFlags = 0;
};

class Image {
public:
  Image() = default;
  ~Image() = default;

  Image(const Image &) = delete;
  Image &operator=(const Image &) = delete;

  Image(Image &&other) noexcept;
  Image &operator=(Image &&other) noexcept;

  bool create(VulkanContext &context, const ImageCreateDescription &desc);

  // For swapchain images
  void wrap(VulkanContext &context, VkImage image, VkImageView imageView,
            const ImageDescription &desc);

  void destroy();

public:
  VkImage handle() const { return m_image; }

  VkImageView view() const { return m_view; }

  VkExtent3D extent3D() const {
    return {m_desc.width, m_desc.height, m_desc.depth};
  }

  VkExtent2D extent2D() const { return {m_desc.width, m_desc.height}; }

  VkFormat format() const { return m_desc.format; }

  uint32_t width() const { return m_desc.width; }

  uint32_t height() const { return m_desc.height; }

  uint32_t depth() const { return m_desc.depth; }

  VkImageLayout layout() const { return m_desc.syncState.layout; }

  ImageDescription description() const { return m_desc; }

  ImageSyncState syncState() const { return m_desc.syncState; }
  void setSyncState(ImageSyncState state);

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