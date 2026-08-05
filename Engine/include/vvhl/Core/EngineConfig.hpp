#pragma once

namespace vvhl
{

struct ApplicationConfig
{
    std::string name = "VVHL";
    uint32_t version = VK_MAKE_VERSION(1, 0, 0);
};

struct InstanceConfig
{
    std::vector<const char*> extensions;
};

struct DeviceConfig
{
    std::vector<const char*> requiredExtensions;
    VkPhysicalDeviceFeatures requiredFeatures{};
};

struct SwapchainConfig
{
    VkPresentModeKHR preferredPresentMode =
        VK_PRESENT_MODE_FIFO_KHR;

    VkFormat preferredFormat =
        VK_FORMAT_B8G8R8A8_SRGB;

    VkColorSpaceKHR preferredColorSpace =
        VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
};

struct EngineConfig
{
    ApplicationConfig application;
    InstanceConfig instance;
    DeviceConfig device;
    SwapchainConfig swapchain;
};

class EngineSettings
{
public:

    static void initialize(const EngineConfig& config);

    static const EngineConfig& get() { return s_config; };

private:

    static EngineConfig s_config;
};

} // namespace vvhl