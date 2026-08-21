
#include <vvhl/Vulkan/Context/VulkanContext.hpp>
#include <vvhl/Core/BuildConfig.hpp>
#include <vvhl/Core/EngineConfig.hpp>
#include <vvhl/Core/GLFWContext.hpp>

namespace vvhl {

bool VulkanContext::initialize(const Window &window) {
  if (volkInitialize() != VK_SUCCESS) {
    LOGE("Failed to initialize Volk");
    return false;
  }

  if (!createInstance())
    return false;

  volkLoadInstance(m_instance);

  if (!createDebugMessenger())
    return false;

  m_surface = window.createSurface(m_instance);

  m_device = std::make_unique<Device>();
  if (!m_device->initialize(m_instance, m_surface))
    return false;

  m_swapchain = std::make_unique<Swapchain>();
  if (!m_swapchain->initialize(*this, m_surface, window.getWidth(),
                               window.getHeight()))
    return false;

  return true;
}

bool VulkanContext::createInstance() {
  const auto config = EngineSettings::get();
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName =
      config.application.name.c_str(); // TODO: Use user defined name here
  appInfo.applicationVersion = config.application.version;
  appInfo.pEngineName = "VVHL";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = EngineSettings::get().instance.apiVersion;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  std::vector<const char *> extensions = getRequiredExtensions();
  if (!checkExtensionCompatibility(extensions)) {
    LOGE("Some requiered extensions are not available");
    return false;
  }

  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  if (BuildConfig::EnableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(ValidationLayers.size());

    createInfo.ppEnabledLayerNames = ValidationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
    LOGE("Failed to create instance");
    return false;
  } else {
    return true;
  }
}

std::vector<const char *> VulkanContext::getRequiredExtensions() {

  std::vector<const char *> extensions;

  std::vector<const char *> userExtensions =
      EngineSettings::get().instance.extensions;
  std::vector<const char *> glfwExtensions =
      GLFWContext::getRequiredExtensions();

  extensions.insert(extensions.end(), userExtensions.begin(),
                    userExtensions.end());
  extensions.insert(extensions.end(), glfwExtensions.begin(),
                    glfwExtensions.end());

  if (BuildConfig::EnableValidationLayers)
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  return extensions;
}

bool VulkanContext::checkExtensionCompatibility(
    const std::vector<const char *> &requested) {
  std::vector<VkExtensionProperties> available = availableExtensions();
  for (const char *extension : requested) {
    bool found = false;

    for (const auto &availableExtension : available) {
      if (strcmp(extension, availableExtension.extensionName) == 0) {
        found = true;
        break;
      }
    }

    if (!found) {
      LOGW("Required Vulkan extension '{}' is not available.", extension);
      return false;
    }
  }

  return true;
}

bool VulkanContext::createDebugMessenger() {
  if (!BuildConfig::EnableValidationLayers)
    return true;

  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

  createInfo.pfnUserCallback = debugCallback;

  if (vkCreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr,
                                     &m_debugMessenger) != VK_SUCCESS) {
    LOGE("Failed to create debug messenger.");
    return false;
  }

  return true;
}

void VulkanContext::destroyDebugMessenger() {
  if (m_debugMessenger != VK_NULL_HANDLE) {
    vkDestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
  }
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanContext::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT,
    const VkDebugUtilsMessengerCallbackDataEXT *callbackData, void *) {
  if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    LOGE(callbackData->pMessage);
  } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    LOGW(callbackData->pMessage);
  } else {
    LOGI(callbackData->pMessage);
  }

  return VK_FALSE;
}

void VulkanContext::destroy() {
  m_device->waitIdle();

  if (m_swapchain)
    m_swapchain->destroy();

  if (m_device)
    m_device->destroy();

  if (m_surface != VK_NULL_HANDLE)
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

  destroyDebugMessenger();

  if (m_instance != VK_NULL_HANDLE)
    vkDestroyInstance(m_instance, nullptr);
}

} // namespace vvhl
