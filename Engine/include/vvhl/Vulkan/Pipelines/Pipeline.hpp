#pragma once

#include <vvhl/Resources/ResourceManager.hpp>
#include <vvhl/Vulkan/Descriptors/DescriptorBinding.hpp>
#include <vvhl/Vulkan/Descriptors/DescriptorPool.hpp>
#include <vvhl/Vulkan/Descriptors/DescriptorSet.hpp>
#include <vvhl/Vulkan/Descriptors/DescriptorSetLayout.hpp>
#include <vvhl/Vulkan/Pipelines/PipelineReflection.hpp>
#include <vvhl/Vulkan/Shaders/Shader.hpp>

namespace vvhl {

using ShaderSource = std::string;

using ShaderBinary = std::vector<uint32_t>;

struct ShaderInput {
  std::variant<ShaderSource, ShaderBinary> code;
  std::string entryPoint = "main";
};

class Pipeline {
public:
  Pipeline() = default;
  ~Pipeline() = default;

  Pipeline(const Pipeline &) = delete;
  Pipeline &operator=(const Pipeline &) = delete;

  virtual void destroy() { destroyBase(); };

  template <WriteDescriptor T>
  bool write(BindingId id, T resourceBind, const uint32_t frameSet) {
    ASSERT(frameSet < EngineSettings::maxFramesInFlight())

    uint32_t setId, bindingId;
    ReflectedDescriptorBinding rdb;

    if (!resolveBindingId(id, setId, bindingId, rdb))
      return false;

    applyAutolayout(resourceBind, rdb.defaultImageLayout);

    m_descriptorGroups[setId].sets[frameSet].write(bindingId, resourceBind,
                                                   rdb.descriptorType);
    return true;
  }

  template <WriteDescriptor T>
  bool writeAllFrames(BindingId id, T resourceBind) {
    uint32_t setId, bindingId;
    ReflectedDescriptorBinding rdb;

    if (!resolveBindingId(id, setId, bindingId, rdb))
      return false;

    applyAutolayout(resourceBind, rdb.defaultImageLayout);

    for (auto &set : m_descriptorGroups[setId].sets)
      set.write(bindingId, resourceBind, rdb.descriptorType);
    return true;
  }

  void updateDescriptors();

  void bind(VkCommandBuffer cmd, uint32_t frameIndex);

public:
  VkPipeline handle() const { return m_pipeline; }
  VkPipelineLayout layout() const { return m_layout; }

protected:
  struct DescriptorSetGroup {
    DescriptorSetLayout layout;
    std::vector<DescriptorSet> sets;
  };

protected:
  void destroyBase();

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

  template <WriteDescriptor T>
  void applyAutolayout(T &resourceBind, VkImageLayout defaultLayout) {

    using DescType = std::decay_t<decltype(resourceBind)>;
    if constexpr (std::is_same_v<DescType, ImageWriteDescriptor> ||
                  std::is_same_v<DescType,
                                 CombinedImageSamplerWriteDescriptor>) {
      if (resourceBind.imageLayout == _autoLayout) {
        resourceBind.imageLayout = defaultLayout;
      }
    } else if constexpr (
        std::is_same_v<DescType, std::vector<ImageWriteDescriptor>> ||
        std::is_same_v<DescType,
                       std::vector<CombinedImageSamplerWriteDescriptor>>) {
      for (auto &rb : resourceBind) {
        if (rb.imageLayout == _autoLayout) {
          rb.imageLayout = defaultLayout;
        }
      }
    }
  }

protected:
  VkPipelineBindPoint m_bindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;

  VkDevice m_device = VK_NULL_HANDLE;
  VkPipeline m_pipeline = VK_NULL_HANDLE;
  VkPipelineLayout m_layout = VK_NULL_HANDLE;

  DescriptorPool *m_pool = nullptr;
  ResourceManager *m_resourceManager = nullptr;

  PipelineReflection m_reflection;
  std::map<uint32_t, DescriptorSetGroup> m_descriptorGroups;
};

} // namespace vvhl