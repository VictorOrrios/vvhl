#include "Sampler.hpp"

namespace vvhl {

bool Sampler::create(Device &device, const SamplerDescription &desc) {
  m_device = &device;

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

  samplerInfo.magFilter = desc.magFilter;
  samplerInfo.minFilter = desc.minFilter;

  samplerInfo.addressModeU = desc.addressModeU;
  samplerInfo.addressModeV = desc.addressModeV;
  samplerInfo.addressModeW = desc.addressModeW;

  samplerInfo.mipmapMode = desc.mipmapMode;

  samplerInfo.mipLodBias = desc.mipLodBias;

  samplerInfo.minLod = desc.minLod;
  samplerInfo.maxLod = desc.maxLod;

  samplerInfo.anisotropyEnable = desc.anisotropyEnable;

  samplerInfo.maxAnisotropy = std::min(
      desc.maxAnisotropy, m_device->properties().limits.maxSamplerAnisotropy);

  samplerInfo.compareEnable = desc.compareEnable;
  samplerInfo.compareOp = desc.compareOp;

  samplerInfo.borderColor = desc.borderColor;

  samplerInfo.unnormalizedCoordinates = desc.unnormalizedCoordinates;

  VkResult result =
      vkCreateSampler(device.handle(), &samplerInfo, nullptr, &m_sampler);

  if (result != VK_SUCCESS) {
    LOGE("Failed to create sampler");
    return false;
  }

  return true;
}

void Sampler::destroy() {
  if (m_sampler == VK_NULL_HANDLE)
    return;

  vkDestroySampler(m_device->handle(), m_sampler, nullptr);

  m_sampler = VK_NULL_HANDLE;
  m_device = VK_NULL_HANDLE;
}

} // namespace vvhl