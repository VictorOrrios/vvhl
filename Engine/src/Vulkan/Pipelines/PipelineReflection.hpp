#pragma once

#include <Vulkan/Shaders/ShaderReflection.hpp>
#include <vvhl/vvhl.hpp>

namespace vvhl {

class PipelineReflection {
public:
  PipelineReflection() = default;
  ~PipelineReflection() { destroy(); };

  PipelineReflection(const PipelineReflection &) = delete;
  PipelineReflection &operator=(const PipelineReflection &) = delete;

  bool add(const ShaderReflection &shaderReflection);

  void destroy();

public:
  const std::vector<DescriptorBinding> &descriptorBindings() const {
    return m_descriptorBindings;
  }

  const PushConstantRange &pushConstants() const {
    return m_pushConstants;
  }

private:
  std::vector<DescriptorBinding> m_descriptorBindings{};

  bool m_pushConstsInitialized = false;
  PushConstantRange m_pushConstants{};
};
} // namespace vvhl
