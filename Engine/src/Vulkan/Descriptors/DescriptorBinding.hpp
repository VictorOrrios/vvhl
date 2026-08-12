#pragma once

#include <vvhl/Resources/ResourceManager.hpp>

namespace vvhl {

struct BindingId {
  std::variant<std::string, std::pair<uint32_t, uint32_t>> id;

  static BindingId name(std::string bindingName) {
    return BindingId{bindingName};
  }

  static BindingId index(uint32_t set, uint32_t binding) {
    return BindingId{std::make_pair(set, binding)};
  }
};

struct BufferBinding {
  BufferHandle handle;
  std::optional<VkDeviceSize> offset;
  std::optional<VkDeviceSize> range;
};

struct ImageBinding {
  ImageHandle handle;
  std::optional<VkImageLayout> layout;
};

struct SamplerBinding {
  SamplerHandle handle;
};

struct CombinedImageSamplerBinding {
  ImageHandle imageHandle;
  SamplerHandle samplerHandle;
  std::optional<VkImageLayout> layout;
};

template <typename T> struct BindingArray {
  std::vector<T> elements;
};

struct DescriptorBinding {
  BindingId binding;
  std::variant<BufferBinding, ImageBinding, SamplerBinding,
               CombinedImageSamplerBinding, BindingArray<BufferBinding>,
               BindingArray<ImageBinding>, BindingArray<SamplerBinding>,
               BindingArray<CombinedImageSamplerBinding>>
      resource;
  std::optional<VkShaderStageFlags> stage;
};

} // namespace vvhl