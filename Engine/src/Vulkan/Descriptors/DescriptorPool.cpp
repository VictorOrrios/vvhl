
#include "DescriptorPool.hpp"

namespace vvhl {

bool DescriptorPool::initialize(VkDevice device,
                                std::span<const PoolSize> poolSizes,
                                uint32_t maxSets,
                                VkDescriptorPoolCreateFlags flags) {

  if (poolSizes.empty() || maxSets == 0) {
    LOGE("Can not initialize an empty descriptor pool")
    return false;
  }

  std::vector<VkDescriptorPoolSize> vkPoolSizes;
  vkPoolSizes.reserve(poolSizes.size());
  for (const auto &size : poolSizes) {
    VkDescriptorPoolSize vkSize{};
    vkSize.type = size.type;
    vkSize.descriptorCount = size.descriptorCount;
    vkPoolSizes.push_back(vkSize);
  }

  VkDescriptorPoolCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  createInfo.poolSizeCount = static_cast<uint32_t>(vkPoolSizes.size());
  createInfo.pPoolSizes = vkPoolSizes.data();
  createInfo.maxSets = maxSets;
  createInfo.flags = flags;

  if (vkCreateDescriptorPool(m_device, &createInfo, nullptr, &m_pool) !=
      VK_SUCCESS) {
    LOGE("Failed to create descriptor pool")
    return false;
  }

  m_device = device;
  return true;
}

void DescriptorPool::destroy() {
  if (m_pool != VK_NULL_HANDLE)
    vkDestroyDescriptorPool(m_device, m_pool, nullptr);

  m_pool = VK_NULL_HANDLE;
  m_device = VK_NULL_HANDLE;
}

void DescriptorPool::reset() {
  if (m_pool != VK_NULL_HANDLE)
    vkResetDescriptorPool(m_device, m_pool, 0);
}

std::vector<VkDescriptorSet>
DescriptorPool::allocate(std::span<const VkDescriptorSetLayout> layouts) {
  if (m_pool == VK_NULL_HANDLE || layouts.empty()) {
    LOGE("Can not allocate descriptor sets if descriptor pool isn't initialized")
    return {};
  }

  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = m_pool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
  allocInfo.pSetLayouts = layouts.data();

  std::vector<VkDescriptorSet> descriptorSets(layouts.size());

  if (vkAllocateDescriptorSets(m_device, &allocInfo, descriptorSets.data()) !=
      VK_SUCCESS) {
    return {};
  }

  return descriptorSets;
}

} // namespace vvhl