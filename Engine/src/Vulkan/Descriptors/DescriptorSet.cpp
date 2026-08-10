#include "DescriptorSet.hpp"
#include <cassert>

namespace vvhl {

void DescriptorSet::initialize(VkDevice device, VkDescriptorSet descriptorSet,
                               ResourceManager *resourceManager) {
  destroy();
  m_device = device;
  m_descriptorSet = descriptorSet;
  m_resourceManager = resourceManager;
}

void DescriptorSet::destroy() {
  m_device = VK_NULL_HANDLE;
  m_descriptorSet = VK_NULL_HANDLE;
  m_resourceManager = nullptr;
  m_pendingWrites.clear();
}

// Write single
void DescriptorSet::write(uint32_t binding,
                          const BufferWriteDescriptor &descriptor) {
  m_pendingWrites.push_back({binding, descriptor});
}

void DescriptorSet::write(uint32_t binding,
                          const ImageWriteDescriptor &descriptor) {
  m_pendingWrites.push_back({binding, descriptor});
}

void DescriptorSet::write(uint32_t binding,
                          const SamplerWriteDescriptor &descriptor) {
  m_pendingWrites.push_back({binding, descriptor});
}

void DescriptorSet::write(
    uint32_t binding, const CombinedImageSamplerWriteDescriptor &descriptor) {
  m_pendingWrites.push_back({binding, descriptor});
}

// Arrays
void DescriptorSet::write(uint32_t binding,
                          std::span<const BufferWriteDescriptor> descriptors) {
  m_pendingWrites.push_back(
      {binding, std::vector<BufferWriteDescriptor>(descriptors.begin(),
                                                   descriptors.end())});
}

// Write array
void DescriptorSet::write(uint32_t binding,
                          std::span<const ImageWriteDescriptor> descriptors) {
  m_pendingWrites.push_back(
      {binding, std::vector<ImageWriteDescriptor>(descriptors.begin(),
                                                  descriptors.end())});
}

void DescriptorSet::write(uint32_t binding,
                          std::span<const SamplerWriteDescriptor> descriptors) {
  m_pendingWrites.push_back(
      {binding, std::vector<SamplerWriteDescriptor>(descriptors.begin(),
                                                    descriptors.end())});
}

void DescriptorSet::write(
    uint32_t binding,
    std::span<const CombinedImageSamplerWriteDescriptor> descriptors) {
  m_pendingWrites.push_back(
      {binding, std::vector<CombinedImageSamplerWriteDescriptor>(
                    descriptors.begin(), descriptors.end())});
}

void DescriptorSet::update() {
  // TODO
}

} // namespace vvhl