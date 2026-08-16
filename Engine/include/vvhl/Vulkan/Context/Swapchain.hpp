#pragma once

#include "Device.hpp"

namespace vvhl {

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

  bool initialize(Device &device, VkSurfaceKHR surface, uint32_t width,
                  uint32_t height);

  void destroy();

  bool recreate();

  VkResult acquireNextImage(VkSemaphore semaphore, VkFence fence,
                            uint32_t &imageIndex);

  VkResult presentImage(uint32_t imageIndex, VkSemaphore waitSemaphore);

public:
  // Vulkan handle
  VkSwapchainKHR handle() const { return m_swapchain; }

  // Images
  uint32_t imageCount() const { return static_cast<uint32_t>(m_images.size()); }

  const std::vector<VkImage> &images() const { return m_images; }

  const std::vector<VkImageView> &imageViews() const { return m_imageViews; }

  VkImage image(uint32_t index) const { return m_images[index]; }

  VkImageView imageView(uint32_t index) const { return m_imageViews[index]; }

  // Config
  SwapchainDetails details() const { return m_details; }

private:
  bool createSwapchain(uint32_t width, uint32_t height);

  bool retrieveImages();

  bool createImageViews();

  VkSurfaceFormatKHR chooseSurfaceFormat() const;

  VkPresentModeKHR choosePresentMode() const;

  VkExtent2D chooseExtent(uint32_t width, uint32_t height) const;

  void destroyImageViews();

  void destroySwapchain();

private:
  Device *m_device = nullptr;

  Device::SwapchainSupport m_swapchainSupport{};

  VkSurfaceKHR m_surface = VK_NULL_HANDLE;

  VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;

  std::vector<VkImage> m_images;

  std::vector<VkImageView> m_imageViews;

  SwapchainDetails m_details;
};
} // namespace vvhl