
#include "Pipeline.hpp"
#include "Vulkan/Descriptors/DescriptorSetLayout.hpp"
#include "Vulkan/Shaders/Shader.hpp"

namespace vvhl {

bool Pipeline::attachmentSetup() {

  auto bindingsBySet = m_reflection.descriptorBindings();
  std::vector<VkDescriptorSetLayout> vkLayouts;

  for (const auto &[set, bindings] : bindingsBySet) {
    DescriptorSetLayout layout;
    if (!layout.initialize(m_device, bindings)) {
      return false;
    }
    vkLayouts.push_back(layout.handle());
    m_descriptorSetLayouts.push_back(std::move(layout));
  }

  if (!createLayout(vkLayouts, m_reflection.pushConstants())) {
    return false;
  }

  return true;
}

void Pipeline::accumulatePoolResources(DescriptorPool &pool) const {
  for (const auto &[set, bindings] : m_reflection.descriptorBindings()) {
    for (const auto &binding : bindings) {
      DescriptorPool::PoolSize poolSize{};
      poolSize.type = binding.descriptorType;
      poolSize.descriptorCount = binding.descriptorCount;
      pool.accumulate(poolSize);
    }
    pool.accumulateSet(1);
  }
}

bool Pipeline::createReflection(std::vector<Shader> shaders) {
  for (auto &shader : shaders) {
    if (!m_reflection.add(shader.reflection())) {
      m_reflection.destroy();
      return false;
    }
  }
  return true;
}

bool Pipeline::createLayout(
    std::span<const VkDescriptorSetLayout> descriptorSetLayouts,
    std::span<const VkPushConstantRange> pushConstantRanges) {

  VkPipelineLayoutCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  createInfo.setLayoutCount =
      static_cast<uint32_t>(descriptorSetLayouts.size());
  createInfo.pSetLayouts = descriptorSetLayouts.data();
  createInfo.pushConstantRangeCount =
      static_cast<uint32_t>(pushConstantRanges.size());
  createInfo.pPushConstantRanges =
      pushConstantRanges.empty() ? nullptr : pushConstantRanges.data();

  if (vkCreatePipelineLayout(m_device, &createInfo, nullptr, &m_layout) !=
      VK_SUCCESS) {
    LOGE("Failed to create pipeline layout")
    return false;
  }

  return true;
}

bool Pipeline::createShader(const ShaderInput &input,
                            const std::string &entryPoint,
                            const VkShaderStageFlagBits stage, Shader &shader) {
  shader.destroy();

  bool success = false;

  std::visit(
      [&](const auto &source) {
        using T = std::decay_t<decltype(source)>;

        if constexpr (std::is_same_v<T, ShaderSource>) {
          success =
              shader.initialize(m_device, source.filePath, stage, entryPoint);
        } else if constexpr (std::is_same_v<T, ShaderBinary>) {
          success =
              shader.initialize(m_device, source.spirv, stage, entryPoint);
        }
      },
      input);

  if (!success) {
    return false;
  }

  return shader.valid();
}

} // namespace vvhl