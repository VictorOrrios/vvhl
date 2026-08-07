#pragma once

#include <Vulkan/Memory/Image.hpp>

namespace vvhl::ImagePresets {

inline constexpr ImageDescription Texture2D{
    .type = VK_IMAGE_TYPE_2D,
    .format = VK_FORMAT_R8G8B8A8_UNORM,

    .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,

    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
};

inline constexpr ImageDescription HDRTexture{
    .type = VK_IMAGE_TYPE_2D,
    .format = VK_FORMAT_R16G16B16A16_SFLOAT,

    .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,

    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
};

inline constexpr ImageDescription RenderTarget{
    .type = VK_IMAGE_TYPE_2D,
    .format = VK_FORMAT_R8G8B8A8_UNORM,

    .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,

    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
};

inline constexpr ImageDescription Depth{
    .type = VK_IMAGE_TYPE_2D,
    .format = VK_FORMAT_D32_SFLOAT,

    .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
             VK_IMAGE_USAGE_SAMPLED_BIT,

    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
};

inline constexpr ImageDescription Storage{
    .type = VK_IMAGE_TYPE_2D,
    .format = VK_FORMAT_R16G16B16A16_SFLOAT,

    .usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,

    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
};

inline constexpr ImageDescription CubeMap{
    .type = VK_IMAGE_TYPE_2D,
    .format = VK_FORMAT_R8G8B8A8_UNORM,

    .arrayLayers = 6,

    .flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,

    .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    
    .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,

};

} // namespace vvhl::ImagePresets