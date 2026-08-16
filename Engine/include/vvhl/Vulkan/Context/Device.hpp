#pragma once

#include <vvhl/vvhl.hpp>
#include <vvhl/Vulkan/Commands/Queue.hpp>

namespace vvhl {

class Device {
public:
  Device() = default;
  ~Device() { destroy(); };

  Device(const Device &) = delete;
  Device &operator=(const Device &) = delete;

  struct SwapchainSupport {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    bool isComplete() const {
      return !formats.empty() && !presentModes.empty();
    }
  };

  bool initialize(const VkInstance &instance, const VkSurfaceKHR &surface);
  void destroy();

  bool waitIdle() const;

  // Vulkan handles
  VkDevice handle() const { return m_device; }
  VkPhysicalDevice physicalHandle() const { return m_physicalDevice; }

  // Queues
  Queue& graphicsQueue() { return m_graphicsQueue; }
  Queue& computeQueue() { return m_computeQueue; }
  Queue& transferQueue() { return m_transferQueue; }
  Queue& presentQueue() { return m_presentQueue; }

  // Queue families
  uint32_t graphicsFamily() const { return *m_queueFamilies.graphics; }
  uint32_t computeFamily() const { return *m_queueFamilies.compute; }
  uint32_t transferFamily() const { return *m_queueFamilies.transfer; }
  uint32_t presentFamily() const { return *m_queueFamilies.present; }

  // GPU information
  const VkPhysicalDeviceProperties &properties() const { return m_properties; }
  const VkPhysicalDeviceFeatures &features() const { return m_features; }
  const VkPhysicalDeviceMemoryProperties &memoryProperties() const {
    return m_memoryProperties;
  }
  SwapchainSupport querySwapchainSupport(VkSurfaceKHR surface) const;

  // Memory
  VmaAllocator allocator() const { return m_allocator; }

private:
  struct QueueFamilyIndices {
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> compute;
    std::optional<uint32_t> transfer;
    std::optional<uint32_t> present;

    bool isComplete() const {
      return graphics.has_value() && present.has_value();
    }
  };

  bool pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);

  uint32_t rateDevice(VkPhysicalDevice device) const;

  bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device,
                                       VkSurfaceKHR surface) const;

  bool checkDeviceExtensionSupport(VkPhysicalDevice device) const;

  SwapchainSupport querySwapchainSupport(VkPhysicalDevice device,
                                         VkSurfaceKHR surface) const;

  bool createLogicalDevice();

  void retrieveQueues();

  bool createAllocator(VkInstance instance);

private:
  inline static constexpr std::array DeviceExtensions{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  inline static constexpr std::array ValidationLayers{
      "VK_LAYER_KHRONOS_validation"};

  VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
  VkDevice m_device = VK_NULL_HANDLE;

  Queue m_graphicsQueue;
  Queue m_computeQueue;
  Queue m_transferQueue;
  Queue m_presentQueue;

  QueueFamilyIndices m_queueFamilies;

  VmaAllocator m_allocator = VK_NULL_HANDLE;

  VkPhysicalDeviceProperties m_properties{};
  VkPhysicalDeviceFeatures m_features{};
  VkPhysicalDeviceMemoryProperties m_memoryProperties{};
};
} // namespace vvhl
