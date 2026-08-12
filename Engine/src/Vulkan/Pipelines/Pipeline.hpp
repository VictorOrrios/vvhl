#pragma once

#include "Vulkan/Descriptors/DescriptorPool.hpp"
#include "Vulkan/Descriptors/DescriptorSetLayout.hpp"
#include "Vulkan/Pipelines/PipelineReflection.hpp"
#include "Vulkan/Shaders/Shader.hpp"
#include <vvhl/vvhl.hpp>

namespace vvhl {

struct ShaderSource {
  std::string filePath;
};

struct ShaderBinary {
  std::vector<uint32_t> spirv;
};

using ShaderInput = std::variant<ShaderSource, ShaderBinary>;

class Pipeline {
public:
  Pipeline() = default;
  ~Pipeline() { destroy(); }

  Pipeline(const Pipeline &) = delete;
  Pipeline &operator=(const Pipeline &) = delete;

  virtual void destroy();

public:
  VkPipeline handle() const { return m_pipeline; }
  VkPipelineLayout layout() const { return m_layout; }

protected:
  bool attachmentSetup();

  void accumulatePoolResources(DescriptorPool &pool) const;

  bool createReflection(const std::vector<Shader> shaders);

  bool createLayout(std::span<const VkDescriptorSetLayout> descriptorSetLayouts,
                    std::span<const VkPushConstantRange> pushConstantRanges);

  bool createShader(const ShaderInput &input, const std::string &entryPoint,
                    const VkShaderStageFlagBits stage, Shader &shader);

  virtual bool createPipeline() = 0;

protected:
  VkDevice m_device = VK_NULL_HANDLE;
  VkPipeline m_pipeline = VK_NULL_HANDLE;
  VkPipelineLayout m_layout = VK_NULL_HANDLE;

  std::vector<DescriptorSetLayout> m_descriptorSetLayouts;
  PipelineReflection m_reflection;
};

} // namespace vvhl