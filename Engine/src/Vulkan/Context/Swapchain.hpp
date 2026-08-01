#pragma once

#include <vvhl/vvhl.hpp>

namespace vvhl
{

class Swapchain{
public:
    Swapchain() = default;
    ~Swapchain();

    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;

    bool initialize();
private:

};
} // namespace vvhl