
#include "DescriptorSetLayout.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vvhl {

bool DescriptorSetLayout::initialize(
    VkDevice device, std::span<const DescriptorBinding> bindings) {

  if (bindings.empty()){
    LOGE("Can not create a DescriptorSetLayout with no bindings")
    return false;
  }

  std::vector<VkDescriptorSetLayoutBinding> vkBindings;
  vkBindings.reserve(bindings.size());

  for (auto &binding : bindings) {
    VkDescriptorSetLayoutBinding vkBinding{};
    vkBinding.descriptorCount = binding.descriptorCount;
    vkBinding.descriptorType = binding.descriptorType;
    vkBinding.binding = binding.binding;
    vkBinding.stageFlags = binding.stageFlags;
    vkBinding.pImmutableSamplers = nullptr;
    vkBindings.push_back(vkBinding);
  }

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = vkBindings.size();
  layoutInfo.pBindings = vkBindings.data();

  if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_handle) !=
      VK_SUCCESS) {
    LOGE("Failed to create descriptor set layout")
    m_handle = VK_NULL_HANDLE;
    return false;
  }

  m_device = device;

  return true;
}

void DescriptorSetLayout::destroy() {
  if (m_handle != VK_NULL_HANDLE)
    vkDestroyDescriptorSetLayout(m_device, m_handle, nullptr);

  m_handle = VK_NULL_HANDLE;
  m_device = VK_NULL_HANDLE;
}

} // namespace vvhl