#include "DescriptorSet.hpp"

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

VkWriteDescriptorSet
DescriptorSet::genericVkWrite(uint32_t binding,
                              const VkDescriptorType descriptorType) const {
  return {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = nullptr,
      .dstSet = m_descriptorSet,
      .dstBinding = binding,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = descriptorType,
      .pImageInfo = nullptr,
      .pBufferInfo = nullptr,
      .pTexelBufferView = nullptr,
  };
}

// Write single
void DescriptorSet::write(uint32_t binding,
                          const BufferWriteDescriptor &descriptor,
                          const VkDescriptorType type) {
  PendingWrite pending;
  pending.bufferInfos.push_back({
      m_resourceManager->buffer(descriptor.handle).handle(),
      descriptor.offset,
      descriptor.range,
  });

  pending.write = genericVkWrite(binding, type);
  pending.write.pBufferInfo = pending.bufferInfos.data();

  m_pendingWrites.push_back(std::move(pending));
}

void DescriptorSet::write(uint32_t binding,
                          const ImageWriteDescriptor &descriptor,
                          const VkDescriptorType type) {
  PendingWrite pending;
  pending.imageInfos.push_back({
      VK_NULL_HANDLE,
      m_resourceManager->image(descriptor.handle).view(),
      descriptor.imageLayout,
  });

  pending.write = genericVkWrite(binding, type);
  pending.write.pImageInfo = pending.imageInfos.data();

  m_pendingWrites.push_back(std::move(pending));
}

void DescriptorSet::write(uint32_t binding,
                          const SamplerWriteDescriptor &descriptor,
                          const VkDescriptorType type) {
  PendingWrite pending;
  pending.imageInfos.push_back({
      m_resourceManager->sampler(descriptor.handle).handle(),
      VK_NULL_HANDLE,
      VK_IMAGE_LAYOUT_UNDEFINED,
  });

  pending.write = genericVkWrite(binding, type);
  pending.write.pImageInfo = pending.imageInfos.data();

  m_pendingWrites.push_back(std::move(pending));
}

void DescriptorSet::write(uint32_t binding,
                          const CombinedImageSamplerWriteDescriptor &descriptor,
                          const VkDescriptorType type) {

  PendingWrite pending;
  pending.imageInfos.push_back({
      m_resourceManager->sampler(descriptor.samplerHandle).handle(),
      m_resourceManager->image(descriptor.imageHandle).view(),
      descriptor.imageLayout,
  });

  pending.write = genericVkWrite(binding, type);
  pending.write.pImageInfo = pending.imageInfos.data();

  m_pendingWrites.push_back(std::move(pending));
}

// Arrays
void DescriptorSet::write(uint32_t binding,
                          std::span<const BufferWriteDescriptor> descriptors,
                          const VkDescriptorType type) {
  if (descriptors.empty())
    return;

  PendingWrite pending;
  pending.bufferInfos.reserve(descriptors.size());

  for (const auto &desc : descriptors) {
    pending.bufferInfos.push_back({
        m_resourceManager->buffer(desc.handle).handle(),
        desc.offset,
        desc.range,
    });
  }

  pending.write = genericVkWrite(binding, type);
  pending.write.pBufferInfo = pending.bufferInfos.data();
  pending.write.descriptorCount =
      static_cast<uint32_t>(pending.bufferInfos.size());

  m_pendingWrites.push_back(std::move(pending));
}

// Write array
void DescriptorSet::write(uint32_t binding,
                          std::span<const ImageWriteDescriptor> descriptors,
                          const VkDescriptorType type) {
  if (descriptors.empty())
    return;

  PendingWrite pending;
  pending.bufferInfos.reserve(descriptors.size());

  for (const auto &desc : descriptors) {
    pending.imageInfos.push_back({
        VK_NULL_HANDLE,
        m_resourceManager->image(desc.handle).view(),
        desc.imageLayout,
    });
  }

  pending.write = genericVkWrite(binding, type);
  pending.write.pImageInfo = pending.imageInfos.data();
  pending.write.descriptorCount =
      static_cast<uint32_t>(pending.imageInfos.size());

  m_pendingWrites.push_back(std::move(pending));
}

void DescriptorSet::write(uint32_t binding,
                          std::span<const SamplerWriteDescriptor> descriptors,
                          const VkDescriptorType type) {
  if (descriptors.empty())
    return;

  PendingWrite pending;
  pending.bufferInfos.reserve(descriptors.size());

  for (const auto &desc : descriptors) {
    pending.imageInfos.push_back({
        m_resourceManager->sampler(desc.handle).handle(),
        VK_NULL_HANDLE,
        VK_IMAGE_LAYOUT_UNDEFINED,
    });
  }

  pending.write = genericVkWrite(binding, type);
  pending.write.pImageInfo = pending.imageInfos.data();
  pending.write.descriptorCount =
      static_cast<uint32_t>(pending.imageInfos.size());

  m_pendingWrites.push_back(std::move(pending));
}

void DescriptorSet::write(
    uint32_t binding,
    std::span<const CombinedImageSamplerWriteDescriptor> descriptors,
    const VkDescriptorType type) {
  if (descriptors.empty())
    return;

  PendingWrite pending;
  pending.bufferInfos.reserve(descriptors.size());

  for (const auto &desc : descriptors) {
    pending.imageInfos.push_back({
        m_resourceManager->sampler(desc.samplerHandle).handle(),
        m_resourceManager->image(desc.imageHandle).view(),
        desc.imageLayout,
    });
  }

  pending.write = genericVkWrite(binding, type);
  pending.write.pImageInfo = pending.imageInfos.data();
  pending.write.descriptorCount =
      static_cast<uint32_t>(pending.imageInfos.size());

  m_pendingWrites.push_back(std::move(pending));
}

void DescriptorSet::update() {
  if (m_pendingWrites.empty())
    return;

  std::vector<VkWriteDescriptorSet> writes;
  writes.reserve(m_pendingWrites.size());

  for (auto &pending : m_pendingWrites) {
    writes.push_back(pending.write);
  }

  vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(writes.size()),
                         writes.data(), 0, nullptr);

  m_pendingWrites.clear();
}

} // namespace vvhl