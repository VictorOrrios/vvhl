
#include <cstdint>
#include <vvhl/Core/EngineConfig.hpp>
#include <vvhl/Vulkan/Context/Swapchain.hpp>

namespace vvhl {

bool Swapchain::initialize(VulkanContext &context, VkSurfaceKHR surface,
                           uint32_t width, uint32_t height) {

  m_context = &context;
  m_device = &context.device();
  m_surface = surface;
  m_swapchainSupport = m_device->querySwapchainSupport(surface);

  if (!createSwapchain(width, height))
    return false;

  if (!retrieveImages())
    return false;

  if (!createImageViews()) {
    destroy();
    return false;
  }

  if (!createSemaphores()) {
    destroy();
    return false;
  }

  wrapImages();

  return true;
}

bool Swapchain::createSwapchain(uint32_t width, uint32_t height) {
  // Choose config
  VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat();
  VkPresentModeKHR presentMode = choosePresentMode();
  VkExtent2D extent = chooseExtent(width, height);

  // Num images in swapchain = min(device.max,device.min+1)
  uint32_t imageCount = m_swapchainSupport.capabilities.minImageCount + 1;
  if (m_swapchainSupport.capabilities.maxImageCount > 0 &&
      imageCount > m_swapchainSupport.capabilities.maxImageCount) {
    imageCount = m_swapchainSupport.capabilities.maxImageCount;
  }

  uint32_t queueFamilyIndices[] = {m_device->graphicsFamily(),
                                   m_device->presentFamily()};

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

  createInfo.surface = m_surface;

  createInfo.minImageCount = imageCount;

  createInfo.imageFormat = surfaceFormat.format;

  createInfo.imageColorSpace = surfaceFormat.colorSpace;

  createInfo.imageExtent = extent;

  createInfo.imageArrayLayers = 1;

  createInfo.imageUsage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

  // Use concurrent mode to share image betewen families if they are not the
  // same
  if (m_device->graphicsFamily() != m_device->presentFamily()) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;

    createInfo.queueFamilyIndexCount = 2;

    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  createInfo.preTransform = m_swapchainSupport.capabilities.currentTransform;

  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

  createInfo.presentMode = presentMode;

  createInfo.clipped = VK_TRUE;

  createInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(m_device->handle(), &createInfo, nullptr,
                           &m_swapchain) != VK_SUCCESS) {
    LOGE("Failed to create swapchain");
    return false;
  }

  m_details.surfaceFormat = surfaceFormat;
  m_details.presentMode = presentMode;
  m_details.extent = extent;

  return true;
}

VkSurfaceFormatKHR Swapchain::chooseSurfaceFormat() const {
  const auto &formats = m_swapchainSupport.formats;

  VkFormat preferredFormat = EngineSettings::get().swapchain.preferredFormat;
  VkColorSpaceKHR preferredColorSpace =
      EngineSettings::get().swapchain.preferredColorSpace;

  for (const auto &format : formats) {
    if (format.format == preferredFormat &&
        format.colorSpace == preferredColorSpace)
      return format;
  }

  LOGW("Preferred surface format or color space for swapchain images not "
       "found, defaulting")

  return formats.front();
}

VkPresentModeKHR Swapchain::choosePresentMode() const {
  const auto &presentModes = m_swapchainSupport.presentModes;

  // FIFO        -> Allways aviable (VSync)
  // MAILBOX     -> Triple buffering, low lag
  // IMMEDIATE   -> Tearing, least lag
  VkPresentModeKHR preferred =
      EngineSettings::get().swapchain.preferredPresentMode;

  for (VkPresentModeKHR mode : presentModes) {
    if (mode == preferred)
      return mode;
  }

  LOGW("Defined present mode ({}) not aviable, defaulting to FIFO",
       uint(preferred))

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::chooseExtent(uint32_t width, uint32_t height) const {
  const auto &capabilities = m_swapchainSupport.capabilities;

  // If it's forced early exit, there is no choice
  if (capabilities.currentExtent.width != UINT32_MAX)
    return capabilities.currentExtent;

  // Clamp extent to device min and max sizes
  VkExtent2D extent{};

  extent.width = std::clamp(width, capabilities.minImageExtent.width,
                            capabilities.maxImageExtent.width);

  extent.height = std::clamp(height, capabilities.minImageExtent.height,
                             capabilities.maxImageExtent.height);

  return extent;
}

bool Swapchain::retrieveImages() {
  uint32_t imageCount = 0;

  vkGetSwapchainImagesKHR(m_device->handle(), m_swapchain, &imageCount,
                          nullptr);

  if (imageCount == 0) {
    LOGE("Swapchain contains no images");
    return false;
  }

  m_images.resize(imageCount);

  if (vkGetSwapchainImagesKHR(m_device->handle(), m_swapchain, &imageCount,
                              m_images.data()) != VK_SUCCESS) {
    LOGE("Failed to retrieve swapchain images");
    return false;
  }

  return true;
}

bool Swapchain::createImageViews() {
  m_imageViews.resize(imageCount());

  for (uint32_t i = 0; i < imageCount(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    createInfo.image = m_images[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = m_details.surfaceFormat.format;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;

    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_device->handle(), &createInfo, nullptr,
                          &m_imageViews[i]) != VK_SUCCESS) {
      LOGE("Failed to create swapchain image view nº{}, deleting all stored "
           "image views",
           i);
      destroyImageViews();
      return false;
    }
  }

  return true;
}

void Swapchain::wrapImages() {
  ASSERT(m_images.size() == m_imageViews.size())
  m_wrapImages.reserve(m_images.size());

  for (uint32_t i = 0; i < m_images.size(); i++) {
    ImageDescription desc;
    desc.type = VK_IMAGE_TYPE_2D;
    desc.viewType = VK_IMAGE_VIEW_TYPE_2D;
    desc.format = m_details.surfaceFormat.format;
    desc.width = m_details.extent.width;
    desc.height = m_details.extent.height;
    desc.depth = 1;

    desc.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    desc.syncState.baseMipLevel = 0;
    desc.syncState.mipLevelCount = 1;
    desc.syncState.baseArrayLayer = 0;
    desc.syncState.arrayLayerCount = 1;
    desc.syncState.layout = VK_IMAGE_LAYOUT_UNDEFINED;
    desc.syncState.access = 0;

    desc.usage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    desc.samples = VK_SAMPLE_COUNT_1_BIT;
    desc.tiling = VK_IMAGE_TILING_OPTIMAL;

    m_wrapImages.push_back({});
    m_wrapImages.back().wrap(*m_context, m_images[i], m_imageViews[i], desc);
  }
}

void Swapchain::destroy() {
  destroySemaphores();
  destroyImageViews();
  destroySwapchain();
  m_images.clear();
  m_imageViews.clear();
  m_wrapImages.clear();
  m_semaphores.clear();
}

bool Swapchain::createSemaphores() {
  m_semaphores.reserve(m_images.size());
  for (uint32_t i = 0; i < m_images.size(); i++) {
    m_semaphores.push_back({});
    if (!m_semaphores.back().initialize(*m_device))
      return false;
  }

  return true;
}

void Swapchain::destroyImageViews() {
  for (VkImageView view : m_imageViews) {
    if (view != VK_NULL_HANDLE)
      vkDestroyImageView(m_device->handle(), view, nullptr);
  }

  m_imageViews.clear();
  m_imageViews.shrink_to_fit();
}

void Swapchain::destroySemaphores() {
  for (auto &semaphore : m_semaphores) {
    semaphore.destroy();
  }
}

void Swapchain::destroySwapchain() {
  if (m_swapchain != VK_NULL_HANDLE)
    vkDestroySwapchainKHR(m_device->handle(), m_swapchain, nullptr);
}

bool Swapchain::recreate() {
  m_device->waitIdle();

  destroy();

  m_swapchainSupport = m_device->querySwapchainSupport(m_surface);

  if (!createSwapchain(m_details.extent.width, m_details.extent.height))
    return false;

  if (!retrieveImages())
    return false;

  if (!createImageViews()) {
    destroy();
    return false;
  }

  return true;
}

VkResult Swapchain::advanceImage(VkSemaphore semaphore, VkFence fence) {
  return vkAcquireNextImageKHR(m_device->handle(), m_swapchain, UINT64_MAX,
                               semaphore, fence, &m_currIndex);
}

VkResult Swapchain::presentImage(VkSemaphore waitSemaphore) {
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &waitSemaphore;

  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &m_swapchain;

  presentInfo.pImageIndices = &m_currIndex;

  return vkQueuePresentKHR(m_device->presentQueue().handle(), &presentInfo);
}

} // namespace vvhl