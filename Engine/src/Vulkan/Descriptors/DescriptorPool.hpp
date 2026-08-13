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

  bool create(VkDevice device, VkDescriptorPoolCreateFlags flags = 0);

  void destroy();
  void reset();

  std::vector<VkDescriptorSet>
  allocate(std::span<const VkDescriptorSetLayout> layouts);

  void accumulate(const PoolSize &poolSize);
  void accumulate(std::span<const PoolSize> poolSizes);
  void accumulateSet(uint32_t numSets);

public:
  VkDescriptorPool handle() const { return m_pool; }

private:
  VkDevice m_device = VK_NULL_HANDLE;
  VkDescriptorPool m_pool = VK_NULL_HANDLE;
  std::unordered_map<VkDescriptorType, uint32_t> m_typeCounts;
  uint32_t m_setCount;
};

} // namespace vvhl