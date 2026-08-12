#pragma once

#include "Vulkan/Descriptors/DescriptorBinding.hpp"
#include "Vulkan/Descriptors/DescriptorPool.hpp"
#include "Vulkan/Descriptors/DescriptorSet.hpp"
#include "Vulkan/Descriptors/DescriptorSetLayout.hpp"
#include "Vulkan/Pipelines/PipelineReflection.hpp"
#include "Vulkan/Shaders/Shader.hpp"
#include <cstdint>
#include <vvhl/vvhl.hpp>

namespace vvhl {

using ShaderSource = std::string;

using ShaderBinary = std::vector<uint32_t>;

using ShaderInput = std::variant<ShaderSource, ShaderBinary>;

class Pipeline {
public:
  Pipeline() = default;
  ~Pipeline() { destroy(); }

  Pipeline(const Pipeline &) = delete;
  Pipeline &operator=(const Pipeline &) = delete;

  virtual void destroy();

  bool writeAllFrames(DescriptorBinding resource);
  bool write(DescriptorBinding resource);
  bool write(DescriptorBinding resource, const uint32_t frameSet);

  void updateDescriptors();

public:
  VkPipeline handle() const { return m_pipeline; }
  VkPipelineLayout layout() const { return m_layout; }

protected:
  struct DescriptorSetGroup {
    DescriptorSetLayout layout;
    std::vector<DescriptorSet> sets;

    VkDescriptorSetLayout vkLayout() const { return layout.handle(); }
  };

protected:
  bool attachmentSetup();

  void accumulatePoolResources(DescriptorPool &pool) const;

  bool createLayout(std::span<const VkDescriptorSetLayout> descriptorSetLayouts,
                    std::span<const VkPushConstantRange> pushConstantRanges);

  bool createShader(const ShaderInput &input, const std::string &entryPoint,
                    const VkShaderStageFlagBits stage, Shader &shader);

  virtual bool createPipeline() = 0;

protected:
  VkDevice m_device = VK_NULL_HANDLE;
  VkPipeline m_pipeline = VK_NULL_HANDLE;
  VkPipelineLayout m_layout = VK_NULL_HANDLE;

  PipelineReflection m_reflection;

  std::map<uint32_t, DescriptorSetGroup> m_descriptorGroups;
};

} // namespace vvhl