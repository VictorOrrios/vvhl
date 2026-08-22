
#pragma once

#include <vvhl/Vulkan/Shaders/ShaderReflection.hpp>

namespace vvhl {

class PipelineReflection {
public:
  PipelineReflection() = default;
  ~PipelineReflection() = default;

  PipelineReflection(const PipelineReflection &) = delete;
  PipelineReflection &operator=(const PipelineReflection &) = delete;

  bool add(const ShaderReflection &shaderReflection);

  void destroy();

public:
  const std::unordered_map<uint32_t, std::vector<ReflectedDescriptorBinding>> &
  descriptorBindings() const {
    return m_descriptorBindings;
  }

  const std::vector<VkPushConstantRange> &pushConstants() const {
    return m_pushConstants;
  }

  bool findByName(std::string name, ReflectedDescriptorBinding &output);
  
  bool findById(uint32_t setId, uint32_t bindingId,
                ReflectedDescriptorBinding &output);

private:
  std::unordered_map<uint32_t, std::vector<ReflectedDescriptorBinding>>
      m_descriptorBindings;
  std::vector<VkPushConstantRange> m_pushConstants;
};

} // namespace vvhl