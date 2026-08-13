#pragma once

#include "Vulkan/Context/Device.hpp"
#include <vvhl/vvhl.hpp>

namespace vvhl {

class Device;

struct SamplerDescription {
  VkFilter magFilter = VK_FILTER_LINEAR;
  VkFilter minFilter = VK_FILTER_LINEAR;

  VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  VkSamplerAddressMode addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

  VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  float mipLodBias = 0.0f;

  float minLod = 0.0f;
  float maxLod = VK_LOD_CLAMP_NONE;

  bool anisotropyEnable = false;
  float maxAnisotropy = 1.0f;

  bool compareEnable = false;
  VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS;

  VkBorderColor borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

  bool unnormalizedCoordinates = false;
};

class Sampler {
public:
  Sampler() = default;
  ~Sampler() { destroy(); };

  Sampler(const Sampler &) = delete;
  Sampler &operator=(const Sampler &) = delete;

  Sampler(Sampler &&other) noexcept;
  Sampler &operator=(Sampler &&other) noexcept;

  bool create(Device &device, const SamplerDescription &desc);

  void destroy();

public:
  VkSampler handle() const { return m_sampler; }
  SamplerDescription description() const { return m_desc; }

private:
  Device *m_device = VK_NULL_HANDLE;
  VkSampler m_sampler = VK_NULL_HANDLE;
  SamplerDescription m_desc{};
};

} // namespace vvhl
