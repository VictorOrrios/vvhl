#pragma once

#include <vvhl/Vulkan/Memory/Sampler.hpp>

namespace vvhl::SamplerPresets {

inline constexpr SamplerDescription LinearRepeat{
    .magFilter = VK_FILTER_LINEAR,
    .minFilter = VK_FILTER_LINEAR,

    .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,

    .anisotropyEnable = true,
    .maxAnisotropy = 16.0f,
};

inline constexpr SamplerDescription LinearClamp{
    .magFilter = VK_FILTER_LINEAR,
    .minFilter = VK_FILTER_LINEAR,

    .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,

    .anisotropyEnable = true,
    .maxAnisotropy = 16.0f,
};

inline constexpr SamplerDescription LinearClampNoAnisotropy{
    .magFilter = VK_FILTER_LINEAR,
    .minFilter = VK_FILTER_LINEAR,

    .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,

    .anisotropyEnable = false,
};

inline constexpr SamplerDescription NearestRepeat{
    .magFilter = VK_FILTER_NEAREST,
    .minFilter = VK_FILTER_NEAREST,

    .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
};

inline constexpr SamplerDescription NearestClamp{
    .magFilter = VK_FILTER_NEAREST,
    .minFilter = VK_FILTER_NEAREST,

    .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
};

inline constexpr SamplerDescription Shadow{
    .magFilter = VK_FILTER_LINEAR,
    .minFilter = VK_FILTER_LINEAR,

    .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
    .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
    .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,

    .compareEnable = true,
    .compareOp = VK_COMPARE_OP_LESS_OR_EQUAL,

    .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
};

} // namespace vvhl::SamplerPresets