#pragma once

#include <vvhl/Vulkan/Shaders/ShaderReflection.hpp>
#include <vvhl/vvhl.hpp>

namespace vvhl {

class DescriptorSetLayout {
public:
  DescriptorSetLayout() = default;
  ~DescriptorSetLayout() { destroy(); };

  DescriptorSetLayout(const DescriptorSetLayout &) = delete;
  DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

  DescriptorSetLayout(DescriptorSetLayout &&other) noexcept;
  DescriptorSetLayout &operator=(DescriptorSetLayout &&other) noexcept;

  bool initialize(VkDevice device, std::span<const ReflectedDescriptorBinding> bindings);

  void destroy();

public:
  VkDescriptorSetLayout handle() const { return m_handle; }

  bool valid() const { return m_handle != VK_NULL_HANDLE; };

private:
  VkDevice m_device = VK_NULL_HANDLE;
  VkDescriptorSetLayout m_handle = VK_NULL_HANDLE;
};
} // namespace vvhl
