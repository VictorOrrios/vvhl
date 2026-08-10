#pragma once

#include <vvhl/Resources/ResourceManager.hpp>
#include <vvhl/vvhl.hpp>

namespace vvhl {

struct BufferWriteDescriptor {
  BufferHandle handle;
  VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  VkDeviceSize offset = 0;
  VkDeviceSize range = VK_WHOLE_SIZE;
};

struct ImageWriteDescriptor {
  ImageHandle handle;
  VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
  VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
};

struct SamplerWriteDescriptor {
  SamplerHandle handle;
  VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
};

struct CombinedImageSamplerWriteDescriptor {
  ImageHandle imageHandle;
  SamplerHandle samplerHandle;
  VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
};

class DescriptorSet {
public:
  DescriptorSet() = default;
  ~DescriptorSet() { destroy(); }

  DescriptorSet(const DescriptorSet &) = delete;
  DescriptorSet &operator=(const DescriptorSet &) = delete;

  void initialize(VkDevice device, VkDescriptorSet descriptorSet,
                  ResourceManager *resourceManager);
  void destroy();

  // Update the descriptor set with pending writes
  void update();

  // Write single
  void write(uint32_t binding, const BufferWriteDescriptor &descriptor);
  void write(uint32_t binding, const ImageWriteDescriptor &descriptor);
  void write(uint32_t binding, const SamplerWriteDescriptor &descriptor);
  void write(uint32_t binding,
             const CombinedImageSamplerWriteDescriptor &descriptor);

  // Write arrays
  void write(uint32_t binding,
             std::span<const BufferWriteDescriptor> descriptors);
  void write(uint32_t binding,
             std::span<const ImageWriteDescriptor> descriptors);
  void write(uint32_t binding,
             std::span<const SamplerWriteDescriptor> descriptors);
  void write(uint32_t binding,
             std::span<const CombinedImageSamplerWriteDescriptor> descriptors);

public:
  VkDescriptorSet handle() const { return m_descriptorSet; }

private:
  using PendingWriteData = std::variant<
      BufferWriteDescriptor, ImageWriteDescriptor, SamplerWriteDescriptor,
      CombinedImageSamplerWriteDescriptor, std::vector<BufferWriteDescriptor>,
      std::vector<ImageWriteDescriptor>, std::vector<SamplerWriteDescriptor>,
      std::vector<CombinedImageSamplerWriteDescriptor>>;

  struct PendingWrite {
    uint32_t binding;
    PendingWriteData data;
  };

private:

private:
  VkDevice m_device = VK_NULL_HANDLE;
  VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
  ResourceManager *m_resourceManager = nullptr;
  std::vector<PendingWrite> m_pendingWrites;
};

} // namespace vvhl