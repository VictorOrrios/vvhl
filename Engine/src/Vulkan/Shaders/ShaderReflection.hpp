#pragma once

#include <vvhl/vvhl.hpp>

namespace vvhl {

struct DescriptorBinding {
  uint32_t set = 0;
  uint32_t binding = 0;

  VkDescriptorType descriptorType{};
  uint32_t descriptorCount = 1;

  VkShaderStageFlags stageFlags = 0;

  std::string name;
};

struct PushConstantRange {
  uint32_t offset = 0;
  uint32_t size = 0;

  VkShaderStageFlags stageFlags = 0;
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
  const std::vector<DescriptorBinding> &descriptorBindings() const {
    return m_descriptorBindings;
  }

  const std::vector<PushConstantRange> &pushConstants() const {
    return m_pushConstants;
  }

  const std::vector<ShaderInterfaceVariable> &inputs() const {
    return m_inputs;
  }

  const std::vector<ShaderInterfaceVariable> &output() const {
    return m_outputs;
  }

private:
  std::vector<DescriptorBinding> m_descriptorBindings;
  std::vector<PushConstantRange> m_pushConstants;
  std::vector<ShaderInterfaceVariable> m_inputs;
  std::vector<ShaderInterfaceVariable> m_outputs;
};

} // namespace vvhl