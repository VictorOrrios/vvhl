
#include <vvhl/Vulkan/Pipelines/Pipeline.hpp>

namespace vvhl {

void Pipeline::updateDescriptors() {
  for (auto &[setNumber, group] : m_descriptorGroups) {
    for (auto &set : group.sets) {
      set.update();
    }
  }
}

void Pipeline::bind(VkCommandBuffer cmd, uint32_t frameIndex) {
  vkCmdBindPipeline(cmd, m_bindPoint, m_pipeline);

  std::vector<VkDescriptorSet> sets;
  sets.reserve(m_descriptorGroups.size());
  for (const auto &[setNumber, group] : m_descriptorGroups) {
    sets.push_back(group.sets[frameIndex].handle());
  }

  if (!sets.empty()) {
    vkCmdBindDescriptorSets(cmd, m_bindPoint, m_layout, 0,
                            static_cast<uint32_t>(sets.size()), sets.data(), 0,
                            nullptr);
  }
}

bool Pipeline::attachmentSetup() {

  std::vector<VkDescriptorSetLayout> vkLayouts;
  auto bindingsBySet = m_reflection.descriptorBindings();

  for (const auto &[set, bindings] : bindingsBySet) {
    DescriptorSetLayout layout;
    if (!layout.initialize(m_device, bindings)) {
      return false;
    }

    vkLayouts.push_back(layout.handle());

    m_descriptorGroups[set] = {.layout = std::move(layout),
                               .sets = std::vector<DescriptorSet>(
                                   EngineSettings::maxFramesInFlight())};

    std::vector<VkDescriptorSetLayout> layoutVector(
        EngineSettings::maxFramesInFlight(), layout.handle());

    std::vector<VkDescriptorSet> vkSets = m_pool->allocate(layoutVector);

    for (uint32_t i = 0; i < EngineSettings::maxFramesInFlight(); i++) {
      m_descriptorGroups[set].sets[i].initialize(m_device, vkSets[i],
                                                 m_resourceManager);
    }
  }

  if (!createLayout(vkLayouts, m_reflection.pushConstants())) {
    return false;
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
                            const VkShaderStageFlagBits stage, Shader &shader) {
  shader.destroy();

  bool success = false;

  std::visit(
      [&](const auto &source) {
        using T = std::decay_t<decltype(source)>;

        if constexpr (std::is_same_v<T, ShaderSource>) {
          success =
              shader.initialize(m_device, source, stage, input.entryPoint);
        } else if constexpr (std::is_same_v<T, ShaderBinary>) {
          success =
              shader.initialize(m_device, source, stage, input.entryPoint);
        }
      },
      input.code);

  if (!success || !shader.valid()) {
    return false;
  }

  if (!m_reflection.add(shader.reflection())) {
    return false;
  }

  return true;
}

bool Pipeline::resolveBindingId(const BindingId &id, uint32_t &set,
                                uint32_t &bindingIndex,
                                ReflectedDescriptorBinding &rdb) {
  bool succes = true;

  std::visit(
      [&](const auto &id) {
        using T = std::decay_t<decltype(id)>;

        if constexpr (std::is_same_v<T, std::string>) {
          if (!m_reflection.findByName(id, rdb)) {
            LOGE("Failed at finding binding named {}", id)
            succes = false;
          } else {
            set = rdb.set;
            bindingIndex = rdb.binding;
          }
        } else if constexpr (std::is_same_v<T, std::pair<uint32_t, uint32_t>>) {
          if (!m_reflection.findById(id.first, id.second, rdb)) {
            LOGE("Failed at finding binding {} at set {}", id.first, id.second)
            succes = false;
          } else {
            set = id.first;
            bindingIndex = id.second;
          }
        }
      },
      id);

  return succes;
}

void Pipeline::destroyBase() {
  if (m_pipeline != VK_NULL_HANDLE) {
    vkDestroyPipeline(m_device, m_pipeline, nullptr);
    m_pipeline = VK_NULL_HANDLE;
  }
  if (m_layout != VK_NULL_HANDLE) {
    vkDestroyPipelineLayout(m_device, m_layout, nullptr);
    m_layout = VK_NULL_HANDLE;
  }
  m_reflection.destroy();
  for (auto &[set, group] : m_descriptorGroups) {
    group.layout.destroy();
    for (auto &set : group.sets)
      set.destroy();
  }
  m_device = VK_NULL_HANDLE;
  m_pipeline = VK_NULL_HANDLE;
  m_layout = VK_NULL_HANDLE;
  m_pool = nullptr;
  m_resourceManager = nullptr;
}

} // namespace vvhl