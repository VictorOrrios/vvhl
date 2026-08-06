#pragma once

#include "./Device.hpp"
#include "./Swapchain.hpp"
#include <vulkan/vulkan_core.h>
#include <vvhl/vvhl.hpp>

namespace vvhl {

class VulkanContext {
public:
  VulkanContext() = default;
  ~VulkanContext() { destroy(); };

  VulkanContext(const VulkanContext &) = delete;
  VulkanContext &operator=(const VulkanContext &) = delete;

  bool initialize(const Window &window);
  void destroy();

  VkInstance instance() const { return m_instance; };
  VkSurfaceKHR surface() const { return m_surface; };
  VmaAllocator allocator() const { return m_device->allocator(); };
  VkDevice device() const { return m_device->handle(); };

  static std::vector<VkExtensionProperties> availableExtensions() {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                           extensions.data());
    return extensions;
  }

private:
  bool createInstance();

  std::vector<const char *> getRequiredExtensions();
  bool checkExtensionCompatibility(const std::vector<const char *> &requested);

  bool createDebugMessenger();
  void destroyDebugMessenger();

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData);

private:
  VkInstance m_instance = VK_NULL_HANDLE;
  VkSurfaceKHR m_surface = VK_NULL_HANDLE;

  inline static constexpr std::array ValidationLayers{
      "VK_LAYER_KHRONOS_validation"};
  VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

  std::unique_ptr<Device> m_device;
  std::unique_ptr<Swapchain> m_swapchain;
};

} // namespace vvhl