
#pragma once

#include <vvhl/vvhl.hpp>
#include <Vulkan/Shaders/ShaderReflection.hpp>

namespace vvhl {

class PipelineReflection {
public:
  PipelineReflection() = default;
  ~PipelineReflection() { destroy(); }

  PipelineReflection(const PipelineReflection &) = delete;
  PipelineReflection &operator=(const PipelineReflection &) = delete;

  bool add(const ShaderReflection &shaderReflection);

  void destroy();

public:
  const std::unordered_map<uint32_t, std::vector<DescriptorBinding>> &descriptorBindings() const {
    return m_descriptorBindings;
  }

  const std::vector<VkPushConstantRange> &pushConstants() const {
    return m_pushConstants;
  }

private:
  std::unordered_map<uint32_t, std::vector<DescriptorBinding>> m_descriptorBindings;
  std::vector<VkPushConstantRange> m_pushConstants;
};

} // namespace vvhl