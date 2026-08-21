#pragma once

#include "Device.hpp"
#include "vvhl/Vulkan/Memory/Image.hpp"
#include "vvhl/Vulkan/Sync/Semaphore.hpp"
#include <vvhl/Vulkan/Context/VulkanContext.hpp>

namespace vvhl {

class Image;
class VulkanContext;

struct SwapchainDetails {
  VkSurfaceFormatKHR surfaceFormat{};
  VkPresentModeKHR presentMode{};
  VkExtent2D extent{};
};

class Swapchain {
public:
  Swapchain() = default;
  ~Swapchain() { destroy(); };

  Swapchain(const Swapchain &) = delete;
  Swapchain &operator=(const Swapchain &) = delete;

  bool initialize(VulkanContext &context, VkSurfaceKHR surface, uint32_t width,
                  uint32_t height);

  void destroy();

  bool recreate();

  VkResult advanceImage(VkSemaphore semaphore, VkFence fence);

  VkResult presentImage(VkSemaphore waitSemaphore);

public:
  VkSwapchainKHR handle() const { return m_swapchain; }

  uint32_t imageCount() const { return static_cast<uint32_t>(m_images.size()); }

  const std::vector<VkImage> &images() const { return m_images; }

  const std::vector<VkImageView> &imageViews() const { return m_imageViews; }

  VkImage image() const { return m_images[m_currIndex]; }

  VkImageView imageView() const { return m_imageViews[m_currIndex]; }

  Image &imageWrap() { return m_wrapImages[m_currIndex]; }

  Semaphore& semaphore() { return m_semaphores[m_currIndex]; }

  SwapchainDetails details() const { return m_details; }

private:
  bool createSwapchain(uint32_t width, uint32_t height);

  bool retrieveImages();

  bool createImageViews();

  bool createSemaphores();

  void wrapImages();

  VkSurfaceFormatKHR chooseSurfaceFormat() const;

  VkPresentModeKHR choosePresentMode() const;

  VkExtent2D chooseExtent(uint32_t width, uint32_t height) const;

  void destroyImageViews();

  void destroySwapchain();

  void destroySemaphores();

private:
  VulkanContext *m_context = nullptr;
  Device *m_device = nullptr;

  VkSurfaceKHR m_surface = VK_NULL_HANDLE;
  VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
  Device::SwapchainSupport m_swapchainSupport{};

  std::vector<Image> m_wrapImages;
  std::vector<VkImage> m_images;
  std::vector<VkImageView> m_imageViews;

  uint32_t m_currIndex = 0;
  std::vector<Semaphore> m_semaphores;

  SwapchainDetails m_details;
};
} // namespace vvhl