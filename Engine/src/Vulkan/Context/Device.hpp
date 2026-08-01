#pragma once

#include <vvhl/vvhl.hpp>

namespace vvhl
{

class Device{
public:
    Device() = default;
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    bool initialize(const VkInstance& instance, const VkSurfaceKHR& surface);

private:

};
} // namespace vvhl
