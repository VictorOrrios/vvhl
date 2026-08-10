#pragma once

#include <vvhl/vvhl.hpp>

namespace vvhl {

class DescriptorPool {
public:
  struct PoolSize {
    VkDescriptorType type;
    uint32_t descriptorCount;
  };

public:
  DescriptorPool() = default;
  ~DescriptorPool() { destroy(); }

  DescriptorPool(const DescriptorPool &) = delete;
  DescriptorPool &operator=(const DescriptorPool &) = delete;

  bool initialize(VkDevice device, std::span<const PoolSize> poolSizes,
                  uint32_t maxSets, VkDescriptorPoolCreateFlags flags = 0);

  void destroy();
  void reset();

  std::vector<VkDescriptorSet>
  allocate(std::span<const VkDescriptorSetLayout> layouts);

public:

  VkDescriptorPool getHandle() const { return m_pool; }

private:
  VkDevice m_device = VK_NULL_HANDLE;
  VkDescriptorPool m_pool = VK_NULL_HANDLE;
};

} // namespace vvhl