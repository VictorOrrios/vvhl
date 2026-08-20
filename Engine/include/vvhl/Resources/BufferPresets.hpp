#pragma once

#include <vvhl/Vulkan/Memory/Buffer.hpp>

namespace vvhl::BufferPresets {

inline constexpr BufferCreateDescription Vertex{
    .usage =
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
};

inline constexpr BufferCreateDescription Index{
    .usage =
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
};

inline constexpr BufferCreateDescription Uniform{
    .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,

    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
    .allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
};

inline constexpr BufferCreateDescription Storage{
    .usage =
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
};

inline constexpr BufferCreateDescription Staging{
    .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,

    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
    .allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
};

inline constexpr BufferCreateDescription Indirect{
    .usage =
        VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
};

} // namespace vvhl::BufferPresets
