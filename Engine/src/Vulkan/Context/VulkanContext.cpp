
#include "VulkanContext.hpp"
#include "Core/GLFWContext/GLFWContext.hpp"

namespace vvhl
{

    bool VulkanContext::initialize(const Window& window){
        if (volkInitialize() != VK_SUCCESS){
            LOGE("Failed to initialize Volk");
            return false;
        }

        if (!createInstance()) return false;

        volkLoadInstance(m_instance);

        if(!createDebugMessenger()) return false;

        m_surface = window.createSurface(m_instance);

        m_device = std::make_unique<Device>();
        if (!m_device->initialize(m_instance, m_surface)) return false;

        m_swapchain = std::make_unique<Swapchain>();
        if (!m_swapchain->initialize(*m_device, m_surface, window.getWidth(), window.getHeight())) return false;

        return true;
    }

    bool VulkanContext::createInstance(){
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "VVHL Application"; // TODO: Use user defined name here
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "VVHL";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_4;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        std::vector<const char*> extensions = getRequiredExtensions();
        if(!checkExtensionCompatibility(extensions)){
            LOGE("Some requiered extensions are not available");
            return false;
        }

        createInfo.enabledExtensionCount =
            static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = 
            extensions.data();

        #ifdef BUILD_DEBUG
            createInfo.enabledLayerCount =
                static_cast<uint32_t>(ValidationLayers.size());

            createInfo.ppEnabledLayerNames =
                ValidationLayers.data();
        #else
            createInfo.enabledLayerCount = 0;
        #endif

        if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
            LOGE("Failed to create instance");
            return false;
        }else{
            return true;
        }
    }

    std::vector<const char*> VulkanContext::getRequiredExtensions(){
        std::vector<const char*> extensions = GLFWContext::getRequiredExtensions();

        #ifdef BUILD_DEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        #endif

        return extensions;
    }

    bool VulkanContext::checkExtensionCompatibility(const std::vector<const char*>& requested){
        std::vector<VkExtensionProperties> available = availableExtensions();
        for (const char* extension : requested){
            bool found = false;

            for (const auto& availableExtension : available){
                if (strcmp(extension, availableExtension.extensionName) == 0){
                    found = true;
                    break;
                }
            }

            if (!found){
                LOGW("Required Vulkan extension '{}' is not available.", extension);
                return false;
            }
        }

        return true;
    }

    bool VulkanContext::createDebugMessenger(){
        #ifdef BUILD_DEBUG
            VkDebugUtilsMessengerCreateInfoEXT createInfo{};
            createInfo.sType =
                VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

            createInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

            createInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

            createInfo.pfnUserCallback = debugCallback;

            if (vkCreateDebugUtilsMessengerEXT(
                m_instance,
                &createInfo,
                nullptr,
                &m_debugMessenger) != VK_SUCCESS)
            {
                LOGE("Failed to create debug messenger.");
                return false;
            }
        #endif

        return true;
    }

    void VulkanContext::destroyDebugMessenger(){
        if (m_debugMessenger != VK_NULL_HANDLE){
            vkDestroyDebugUtilsMessengerEXT(
                m_instance,
                m_debugMessenger,
                nullptr);
        }
    }


    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanContext::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void*)
    {
        if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT){
            LOGE(callbackData->pMessage);
        }
        else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT){
            LOGW(callbackData->pMessage);
        }
        else{
            LOGI(callbackData->pMessage);
        }

        return VK_FALSE;
    }

    void VulkanContext::destroy(){
        m_device->waitIdle();
        
        if (m_swapchain)
            m_swapchain.reset();

        if (m_device)
            m_device.reset();

        if (m_surface != VK_NULL_HANDLE)
            vkDestroySurfaceKHR(m_instance,m_surface,nullptr);
        
        #ifdef BUILD_DEBUG
        destroyDebugMessenger();
        #endif

        if (m_instance != VK_NULL_HANDLE)
            vkDestroyInstance(m_instance, nullptr);
    }

} // namespace vvhl
