
#include "DescriptorSetLayout.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vvhl {

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout &&other) noexcept
    : m_device(other.m_device), m_handle(other.m_handle) {
  other.m_device = VK_NULL_HANDLE;
  other.m_handle = VK_NULL_HANDLE;
}

DescriptorSetLayout &
DescriptorSetLayout::operator=(DescriptorSetLayout &&other) noexcept {
  if (this != &other) {
    destroy();
    m_device = other.m_device;
    m_handle = other.m_handle;
    other.m_device = VK_NULL_HANDLE;
    other.m_handle = VK_NULL_HANDLE;
  }
  return *this;
}

bool DescriptorSetLayout::initialize(
    VkDevice device, std::span<const DescriptorBinding> bindings) {

  if (bindings.empty()) {
    LOGE("Can not create a DescriptorSetLayout with no bindings")
    return false;
  }

  std::vector<VkDescriptorSetLayoutBinding> vkBindings;
  vkBindings.reserve(bindings.size());

  for (auto &binding : bindings) {
    VkDescriptorSetLayoutBinding vkBinding{};
    vkBinding.binding = binding.binding;
    vkBinding.descriptorType = binding.descriptorType;
    vkBinding.descriptorCount = binding.descriptorCount;
    vkBinding.stageFlags = binding.stageFlags;
    vkBinding.pImmutableSamplers = nullptr;

    vkBindings.push_back(vkBinding);
  }

  VkDescriptorSetLayoutCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  createInfo.bindingCount = static_cast<uint32_t>(vkBindings.size());
  createInfo.pBindings = vkBindings.data();

  if (vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &m_handle) !=
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