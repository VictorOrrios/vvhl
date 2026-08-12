#pragma once

#include <vulkan/vulkan_core.h>
#include <vvhl/vvhl.hpp>

namespace vvhl {

struct ReflectedDescriptorBinding {
  uint32_t set = 0;
  uint32_t binding = 0;
  std::string name;

  VkDescriptorType descriptorType{};
  uint32_t descriptorCount = 1;
  VkShaderStageFlags stageFlags = 0;
  VkImageLayout defaultImageLayout;
};

struct ShaderInterfaceVariable {
  uint32_t location = 0;
  VkFormat format = VK_FORMAT_UNDEFINED;
  std::string name;
};

class ShaderReflection {
public:
  ShaderReflection() = default;
  ~ShaderReflection() { destroy(); };

  ShaderReflection(const ShaderReflection &) = delete;
  ShaderReflection &operator=(const ShaderReflection &) = delete;

  bool initialize(std::span<const uint32_t> spirv, VkShaderStageFlagBits stage);

  void destroy();

public:
  const std::vector<ReflectedDescriptorBinding> &descriptorBindings() const {
    return m_descriptorBindings;
  }

  const std::vector<VkPushConstantRange> &pushConstants() const {
    return m_pushConstants;
  }

  const std::vector<ShaderInterfaceVariable> &inputs() const {
    return m_inputs;
  }

  const std::vector<ShaderInterfaceVariable> &output() const {
    return m_outputs;
  }

private:
  VkImageLayout deduceImageLayout(const SpvReflectDescriptorBinding * binding) const;

private:
  std::vector<ReflectedDescriptorBinding> m_descriptorBindings;
  std::vector<VkPushConstantRange> m_pushConstants;
  std::vector<ShaderInterfaceVariable> m_inputs;
  std::vector<ShaderInterfaceVariable> m_outputs;
};

} // namespace vvhl