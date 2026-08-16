
#include <vvhl/Vulkan/Context/Device.hpp>
#include <vvhl/Core/EngineConfig.hpp>

namespace vvhl {

bool Device::initialize(const VkInstance &instance,
                        const VkSurfaceKHR &surface) {
  if (!pickPhysicalDevice(instance, surface))
    return false;

  if (!createLogicalDevice())
    return false;

  retrieveQueues();

  if (!createAllocator(instance))
    return false;

  volkLoadDevice(m_device);

  return true;
}

bool Device::pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    LOGE("No Vulkan compatible GPU found.");
    return false;
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  uint32_t bestScore = 0;
  for (VkPhysicalDevice device : devices) {
    if (!isDeviceSuitable(device, surface))
      continue;

    uint32_t score = rateDevice(device);

    if (score > bestScore) {
      bestScore = score;
      m_physicalDevice = device;
    }
  }

  if (m_physicalDevice == VK_NULL_HANDLE) {
    LOGE("No suitable GPU found.");
    return false;
  }

  vkGetPhysicalDeviceProperties(m_physicalDevice, &m_properties);

  vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_features);

  vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_memoryProperties);

  m_queueFamilies = findQueueFamilies(m_physicalDevice, surface);

  LOGI("Selected GPU: {}", m_properties.deviceName);
  return true;
}

uint32_t Device::rateDevice(VkPhysicalDevice device) const {
  uint32_t score = 0;
  VkPhysicalDeviceProperties properties{};

  vkGetPhysicalDeviceProperties(device, &properties);

  switch (properties.deviceType) {
  case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
    score += 1000;
    break;

  case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
    score += 500;
    break;

  default:
    break;
  }

  score += properties.limits.maxImageDimension2D;

  return score;
}

bool Device::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
  QueueFamilyIndices indices = findQueueFamilies(device, surface);

  if (!indices.isComplete())
    return false;

  if (!checkDeviceExtensionSupport(device))
    return false;

  if (!querySwapchainSupport(device, surface).isComplete())
    return false;

  return true;
}

Device::QueueFamilyIndices
Device::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) const {

  // TODO: Make smarter choices when searching for queues, potential for
  // parallel computing/transfering

  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> families(queueFamilyCount);

  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           families.data());

  for (uint32_t i = 0; i < queueFamilyCount; ++i) {
    const auto &family = families[i];

    if ((family.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
        !indices.graphics.has_value()) {
      indices.graphics = i;
    }

    if ((family.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
        !indices.compute.has_value()) {
      indices.compute = i;
    }

    if ((family.queueFlags & VK_QUEUE_TRANSFER_BIT) &&
        !indices.transfer.has_value()) {
      indices.transfer = i;
    }

    VkBool32 presentSupport = VK_FALSE;

    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

    if (presentSupport && !indices.present.has_value()) {
      indices.present = i;
    }
  }

  if (!indices.compute)
    indices.compute = indices.graphics;

  if (!indices.transfer)
    indices.transfer = indices.graphics;

  return indices;
}

bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device) const {
  uint32_t extensionCount = 0;

  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);

  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       availableExtensions.data());

  std::vector<const char *> reqExt =
      EngineSettings::get().device.requiredExtensions;
  reqExt.insert(reqExt.end(), DeviceExtensions.begin(), DeviceExtensions.end());

  for (const char *required : reqExt) {
    bool found = false;

    for (const auto &available : availableExtensions) {
      if (strcmp(required, available.extensionName) == 0) {
        found = true;
        break;
      }
    }

    if (!found)
      return false;
  }

  return true;
}

Device::SwapchainSupport
Device::querySwapchainSupport(VkSurfaceKHR surface) const {
  return querySwapchainSupport(m_physicalDevice, surface);
}

Device::SwapchainSupport
Device::querySwapchainSupport(VkPhysicalDevice device,
                              VkSurfaceKHR surface) const {
  SwapchainSupport support;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                            &support.capabilities);

  uint32_t formatCount = 0;

  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  if (formatCount > 0) {
    support.formats.resize(formatCount);

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         support.formats.data());
  }

  uint32_t presentModeCount = 0;

  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
                                            nullptr);

  if (presentModeCount > 0) {
    support.presentModes.resize(presentModeCount);

    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &presentModeCount, support.presentModes.data());
  }

  return support;
}

bool Device::createLogicalDevice() {
  // Remove duplicates
  std::set<uint32_t> uniqueQueueFamilies = {
      *m_queueFamilies.graphics, *m_queueFamilies.present,
      *m_queueFamilies.compute, *m_queueFamilies.transfer};

  float queuePriority = 1.0f;

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

  for (uint32_t family : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

    queueCreateInfo.queueFamilyIndex = family;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures enabledFeatures =
      EngineSettings::get().device.requiredFeatures;

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.pEnabledFeatures = &enabledFeatures;
  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(DeviceExtensions.size());
  createInfo.ppEnabledExtensionNames = DeviceExtensions.data();

  if (BuildConfig::EnableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(ValidationLayers.size());
    createInfo.ppEnabledLayerNames = ValidationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) !=
      VK_SUCCESS) {
    LOGE("Failed to create logical device");
    return false;
  }

  return true;
}

void Device::retrieveQueues() {
  auto getQueue = [this](uint32_t family, Queue &queue) {
    VkQueue handle = VK_NULL_HANDLE;

    vkGetDeviceQueue(m_device, family, 0, &handle);

    queue.initialize(handle, family);
  };

  if (m_queueFamilies.graphics)
    getQueue(*m_queueFamilies.graphics, m_graphicsQueue);

  if (m_queueFamilies.compute)
    getQueue(*m_queueFamilies.compute, m_computeQueue);

  if (m_queueFamilies.transfer)
    getQueue(*m_queueFamilies.transfer, m_transferQueue);

  if (m_queueFamilies.present)
    getQueue(*m_queueFamilies.present, m_presentQueue);
}

bool Device::createAllocator(VkInstance instance) {
  VmaVulkanFunctions functions{};
  functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
  functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

  VmaAllocatorCreateInfo createInfo{};
  createInfo.instance = instance;
  createInfo.physicalDevice = m_physicalDevice;
  createInfo.device = m_device;
  createInfo.vulkanApiVersion = VK_API_VERSION_1_4;
  createInfo.pVulkanFunctions = &functions;

  if (vmaCreateAllocator(&createInfo, &m_allocator) != VK_SUCCESS) {
    LOGE("Failed to create VMA allocator");
    return false;
  }

  return true;
}

bool Device::waitIdle() const {
  return vkDeviceWaitIdle(m_device) == VK_SUCCESS;
}

void Device::destroy() {
  waitIdle();

  if (m_allocator) {
    vmaDestroyAllocator(m_allocator);
    m_allocator = VK_NULL_HANDLE;
  }

  if (m_device) {
    vkDestroyDevice(m_device, nullptr);
    m_device = VK_NULL_HANDLE;
  }
}

}; // namespace vvhl