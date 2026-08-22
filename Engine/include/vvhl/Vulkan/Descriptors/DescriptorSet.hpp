#pragma once

#include "DescriptorBinding.hpp"
#include <vvhl/Resources/ResourceManager.hpp>

namespace vvhl {

class DescriptorSet {
public:
  DescriptorSet() = default;
  ~DescriptorSet() = default;

  DescriptorSet(const DescriptorSet &) = delete;
  DescriptorSet &operator=(const DescriptorSet &) = delete;

  void initialize(VkDevice device, VkDescriptorSet descriptorSet,
                  ResourceManager *resourceManager);
  void destroy();

  // Update the descriptor set with pending writes
  void update();

  // Write single
  void write(uint32_t binding, const BufferWriteDescriptor &descriptor,
             const VkDescriptorType type);
  void write(uint32_t binding, const ImageWriteDescriptor &descriptor,
             const VkDescriptorType type);
  void write(uint32_t binding, const SamplerWriteDescriptor &descriptor,
             const VkDescriptorType type);
  void write(uint32_t binding,
             const CombinedImageSamplerWriteDescriptor &descriptor,
             const VkDescriptorType type);

  // Write arrays
  void write(uint32_t binding,
             std::span<const BufferWriteDescriptor> descriptors,
             const VkDescriptorType type);
  void write(uint32_t binding,
             std::span<const ImageWriteDescriptor> descriptors,
             const VkDescriptorType type);
  void write(uint32_t binding,
             std::span<const SamplerWriteDescriptor> descriptors,
             const VkDescriptorType type);
  void write(uint32_t binding,
             std::span<const CombinedImageSamplerWriteDescriptor> descriptors,
             const VkDescriptorType type);

public:
  VkDescriptorSet handle() const { return m_descriptorSet; }
  VkWriteDescriptorSet
  genericVkWrite(uint32_t binding, const VkDescriptorType descriptorType) const;

private:
  struct PendingWrite {
    VkWriteDescriptorSet write;
    std::vector<VkDescriptorBufferInfo> bufferInfos;
    std::vector<VkDescriptorImageInfo> imageInfos;
  };

private:
  VkDevice m_device = VK_NULL_HANDLE;
  VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
  ResourceManager *m_resourceManager = nullptr;
  std::vector<PendingWrite> m_pendingWrites;
};

} // namespace vvhl