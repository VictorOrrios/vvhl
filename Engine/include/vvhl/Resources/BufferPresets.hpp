#pragma once

#include <Vulkan/Memory/Buffer.hpp>

namespace vvhl::BufferPresets {

inline constexpr BufferDescription Vertex{
    .usage =
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
};

inline constexpr BufferDescription Index{
    .usage =
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
};

inline constexpr BufferDescription Uniform{
    .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,

    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
    .allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
};

inline constexpr BufferDescription Storage{
    .usage =
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
};

inline constexpr BufferDescription Staging{
    .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,

    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
    .allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
};

inline constexpr BufferDescription Indirect{
    .usage =
        VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
};

} // namespace vvhl::BufferPresets
