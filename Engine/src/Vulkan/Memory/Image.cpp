
#include "vvhl/Core/Logger.hpp"
#include "vvhl/Core/Window.hpp"
#include <vulkan/vulkan_core.h>
#include <vvhl/Vulkan/Memory/Image.hpp>

namespace vvhl {

Image::Image(Image &&other) noexcept
    : m_context(other.m_context), m_image(other.m_image), m_view(other.m_view),
      m_allocation(other.m_allocation),
      m_allocationInfo(other.m_allocationInfo), m_desc(other.m_desc) {

  other.m_context = nullptr;
  other.m_image = VK_NULL_HANDLE;
  other.m_view = VK_NULL_HANDLE;
  other.m_allocation = nullptr;
  other.m_allocationInfo = {};
  other.m_desc = {};
}

bool Image::create(VulkanContext &context, const ImageCreateDescription &desc) {
  m_context = &context;

  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = desc.type;
  imageInfo.extent = {desc.width, desc.height, desc.depth};
  imageInfo.mipLevels = desc.mipLevels;
  imageInfo.arrayLayers = desc.arrayLayers;
  imageInfo.format = desc.format;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = desc.usage;
  imageInfo.samples = desc.samples;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.flags = desc.createFlags;

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = desc.memoryUsage;
  allocInfo.flags = desc.allocationFlags;

  VkResult result = vmaCreateImage(context.allocator(), &imageInfo, &allocInfo,
                                   &m_image, &m_allocation, &m_allocationInfo);

  if (result != VK_SUCCESS) {
    LOGE("Failed to create image {}", string_VkResult(result));
    return false;
  }

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = m_image;
  viewInfo.viewType = desc.viewType;
  viewInfo.format = desc.format;

  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = desc.mipLevels;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = desc.arrayLayers;

  if (desc.aspectMask == _autoAspectMask) {
    viewInfo.subresourceRange.aspectMask = getAspectMask(desc.format);
  } else {
    viewInfo.subresourceRange.aspectMask = desc.aspectMask;
  }

  result =
      vkCreateImageView(context.deviceHandle(), &viewInfo, nullptr, &m_view);

  if (result != VK_SUCCESS) {
    LOGE("Failed to create image view");

    vmaDestroyImage(context.allocator(), m_image, m_allocation);

    m_image = VK_NULL_HANDLE;
    m_allocation = nullptr;
    m_desc = {};

    return false;
  }

  m_desc.type = desc.type;
  m_desc.format = desc.format;
  m_desc.width = desc.width;
  m_desc.height = desc.height;
  m_desc.depth = desc.depth;
  m_desc.samples = desc.samples;
  m_desc.tiling = desc.tiling;
  m_desc.createFlags = desc.createFlags;
  m_desc.usage = desc.usage;
  m_desc.viewType = desc.viewType;
  m_desc.aspectMask = desc.aspectMask;
  m_desc.memoryUsage = desc.memoryUsage;
  m_desc.allocationFlags = desc.allocationFlags;

  return true;
}

void Image::wrap(VulkanContext &context, VkImage image, VkImageView imageView,
                 const ImageDescription &desc) {
  ASSERT(m_image == VK_NULL_HANDLE)
  m_context = &context;
  m_image = image;
  m_view = imageView;
  m_desc = desc;
}

VkImageAspectFlags Image::getAspectMask(VkFormat format) {
  switch (format) {
  // Depth only
  case VK_FORMAT_D16_UNORM:
  case VK_FORMAT_X8_D24_UNORM_PACK32:
  case VK_FORMAT_D32_SFLOAT:
    return VK_IMAGE_ASPECT_DEPTH_BIT;

  // Depth + Stencil
  case VK_FORMAT_D16_UNORM_S8_UINT:
  case VK_FORMAT_D24_UNORM_S8_UINT:
  case VK_FORMAT_D32_SFLOAT_S8_UINT:
    return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

  // Stencil only
  case VK_FORMAT_S8_UINT:
    return VK_IMAGE_ASPECT_STENCIL_BIT;

  // Everything else
  default:
    return VK_IMAGE_ASPECT_COLOR_BIT;
  }
}

void Image::destroy() {
  if (m_view != VK_NULL_HANDLE) {
    vkDestroyImageView(m_context->deviceHandle(), m_view, nullptr);
    m_view = VK_NULL_HANDLE;
  }

  if (m_image != VK_NULL_HANDLE) {
    vmaDestroyImage(m_context->allocator(), m_image, m_allocation);
    m_image = VK_NULL_HANDLE;
  }

  m_allocation = nullptr;
  m_allocationInfo = {};

  m_desc = {};

  m_context = nullptr;
}

void Image::setSyncState(ImageSyncState state) {
  m_desc.syncState.access = state.access;
  m_desc.syncState.baseMipLevel = state.baseMipLevel;
  m_desc.syncState.mipLevelCount = state.mipLevelCount;
  m_desc.syncState.baseArrayLayer = state.baseArrayLayer;
  m_desc.syncState.arrayLayerCount = state.arrayLayerCount;
  m_desc.syncState.layout = state.layout;
}

} // namespace vvhl
