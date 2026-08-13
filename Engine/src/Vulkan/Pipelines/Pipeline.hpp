#pragma once

#include "Vulkan/Descriptors/DescriptorBinding.hpp"
#include "Vulkan/Descriptors/DescriptorPool.hpp"
#include "Vulkan/Descriptors/DescriptorSet.hpp"
#include "Vulkan/Descriptors/DescriptorSetLayout.hpp"
#include "Vulkan/Pipelines/PipelineReflection.hpp"
#include "Vulkan/Shaders/Shader.hpp"
#include "vvhl/Resources/ResourceManager.hpp"
#include <vvhl/vvhl.hpp>

namespace vvhl {

using ShaderSource = std::string;

using ShaderBinary = std::vector<uint32_t>;

struct ShaderInput {
  std::string entryPoint;
  std::variant<ShaderSource, ShaderBinary> code;
};

class Pipeline {
public:
  Pipeline() = default;
  ~Pipeline() { destroy(); }

  Pipeline(const Pipeline &) = delete;
  Pipeline &operator=(const Pipeline &) = delete;

  virtual void destroy();

  template <WriteDescriptor T>
  bool write(BindingId id, T resourceBind, const uint32_t frameSet);

  template <WriteDescriptor T>
  bool writeAllFrames(BindingId id, T resourceBind);

  void updateDescriptors();

public:
  VkPipeline handle() const { return m_pipeline; }
  VkPipelineLayout layout() const { return m_layout; }

protected:
  struct DescriptorSetGroup {
    DescriptorSetLayout layout;
    std::vector<DescriptorSet> sets;
  };

protected:
  bool attachmentSetup();

  bool createLayout(std::span<const VkDescriptorSetLayout> descriptorSetLayouts,
                    std::span<const VkPushConstantRange> pushConstantRanges);

  bool createShader(const ShaderInput &input, const VkShaderStageFlagBits stage,
                    Shader &shader);

  virtual bool createPipeline() = 0;

private:
  bool resolveBindingId(const BindingId &id, uint32_t &set,
                        uint32_t &bindingIndex,
                        ReflectedDescriptorBinding &rdb);

protected:
  VkDevice m_device = VK_NULL_HANDLE;
  VkPipeline m_pipeline = VK_NULL_HANDLE;
  VkPipelineLayout m_layout = VK_NULL_HANDLE;

  DescriptorPool *m_pool = nullptr;
  ResourceManager *m_resourceManager = nullptr;

  PipelineReflection m_reflection;
  std::map<uint32_t, DescriptorSetGroup> m_descriptorGroups;
};

} // namespace vvhl