#pragma once

#include "Vulkan/Descriptors/DescriptorBinding.hpp"
#include "Vulkan/Descriptors/DescriptorPool.hpp"
#include "Vulkan/Descriptors/DescriptorSet.hpp"
#include "Vulkan/Descriptors/DescriptorSetLayout.hpp"
#include "Vulkan/Pipelines/PipelineReflection.hpp"
#include "Vulkan/Shaders/Shader.hpp"
#include <cstdint>
#include <string>
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

  bool write(DescriptorBinding resourceBind, const uint32_t frameSet);
  bool writeAllFrames(DescriptorBinding resourceBind);

  
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

  void accumulatePoolResources(DescriptorPool &pool) const;

  bool createLayout(std::span<const VkDescriptorSetLayout> descriptorSetLayouts,
                    std::span<const VkPushConstantRange> pushConstantRanges);

  bool createShader(const ShaderInput &input, const std::string &entryPoint,
                    const VkShaderStageFlagBits stage, Shader &shader);

  virtual bool createPipeline() = 0;

private:
  bool resolveBinding(const BindingId &binding, uint32_t &set,
                      uint32_t &bindingIndex);

  void writeResolvedBinding(const ReflectedDescriptorBinding &rdb,
                            const DescriptorBinding &binding,
                            uint32_t frameSet);

  void writeBuffer(uint32_t set, uint32_t binding, uint32_t frameSet,
                   const ReflectedDescriptorBinding &rdb,
                   const BufferBinding &buffer);
  void writeImage(uint32_t set, uint32_t binding, uint32_t frameSet,
                  const ReflectedDescriptorBinding &rdb,
                  const ImageBinding &image);
  void writeSampler(uint32_t set, uint32_t binding, uint32_t frameSet,
                    const ReflectedDescriptorBinding &rdb,
                    const SamplerBinding &sampler);
  void writeCombined(uint32_t set, uint32_t binding, uint32_t frameSet,
                     const ReflectedDescriptorBinding &rdb,
                     const CombinedImageSamplerBinding &combined);

protected:
  VkDevice m_device = VK_NULL_HANDLE;
  VkPipeline m_pipeline = VK_NULL_HANDLE;
  VkPipelineLayout m_layout = VK_NULL_HANDLE;

  PipelineReflection m_reflection;

  std::map<uint32_t, DescriptorSetGroup> m_descriptorGroups;
};

} // namespace vvhl