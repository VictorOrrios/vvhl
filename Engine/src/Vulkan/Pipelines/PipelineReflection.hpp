#pragma once

#include <Vulkan/Shaders/ShaderReflection.hpp>
#include <cstdint>
#include <vector>
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

  const std::vector<ShaderInterfaceVariable> &inputs() const {
    return m_inputs;
  }

  const std::vector<ShaderInterfaceVariable> &output() const {
    return m_outputs;
  }

private:
  std::vector<DescriptorBinding> m_descriptorBindings{};

  bool m_pushConstsInitialized = false;
  PushConstantRange m_pushConstants{};

  // TODO: Figure out of I need io objects or if it's defined by the user
  std::vector<ShaderInterfaceVariable> m_inputs{};
  std::vector<ShaderInterfaceVariable> m_outputs{};
};
} // namespace vvhl
