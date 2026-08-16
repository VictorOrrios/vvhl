#pragma once

#include <vvhl/Resources/ResourceManager.hpp>

namespace vvhl {

using BindingId = std::variant<std::string, std::pair<uint32_t, uint32_t>>;

constexpr VkImageLayout _autoLayout = VK_IMAGE_LAYOUT_MAX_ENUM;

struct BufferWriteDescriptor {
  BufferHandle handle;
  VkDeviceSize offset = 0;
  VkDeviceSize range = VK_WHOLE_SIZE;
};

struct ImageWriteDescriptor {
  ImageHandle handle;
  VkImageLayout imageLayout = _autoLayout;
};

struct SamplerWriteDescriptor {
  SamplerHandle handle;
};

struct CombinedImageSamplerWriteDescriptor {
  ImageHandle imageHandle;
  SamplerHandle samplerHandle;
  VkImageLayout imageLayout = _autoLayout;
};

template <typename T>
concept WriteDescriptor = std::same_as<T, BufferWriteDescriptor> ||
                          std::same_as<T, ImageWriteDescriptor> ||
                          std::same_as<T, SamplerWriteDescriptor> ||
                          std::same_as<T, CombinedImageSamplerWriteDescriptor> ||
                          std::same_as<T, std::vector<BufferWriteDescriptor>> ||
                          std::same_as<T, std::vector<ImageWriteDescriptor>> ||
                          std::same_as<T, std::vector<SamplerWriteDescriptor>> ||
                          std::same_as<T, std::vector<CombinedImageSamplerWriteDescriptor>>;

} // namespace vvhl