
#include <vvhl/Vulkan/Memory/Image.hpp>

namespace vvhl {

bool Image::create(VulkanContext &context, const ImageDescription &desc) {
  m_context = &context;

  m_desc = desc;

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
    LOGE("Failed to create image");
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

  viewInfo.subresourceRange.aspectMask = getAspectMask(desc.format);

  result = vkCreateImageView(context.deviceHandle(), &viewInfo, nullptr, &m_view);

  if (result != VK_SUCCESS) {
    LOGE("Failed to create image view");

    vmaDestroyImage(context.allocator(), m_image, m_allocation);

    m_image = VK_NULL_HANDLE;
    m_allocation = nullptr;
    m_desc = {};

    return false;
  }

  return true;
}

VkImageAspectFlags getAspectMask(VkFormat format) {
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

} // namespace vvhl
