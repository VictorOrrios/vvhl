
#include <vvhl/Vulkan/Pipelines/ComputePipeline.hpp>

namespace vvhl {
bool ComputePipeline::initialize(const CreateInfo &createInfo) {
  destroy();

  m_bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

  if (createInfo.renderPass == nullptr ||
      createInfo.renderPass->context().deviceHandle() == VK_NULL_HANDLE) {
    LOGE("Pipeline created with empty references")
    return false;
  }

  m_device = createInfo.renderPass->context().deviceHandle();
  m_flags = createInfo.flags;
  m_pool = &createInfo.renderPass->descriptorPool();
  m_resourceManager = &createInfo.renderPass->resourceManager();

  if (!createShader(createInfo.shaderInput, VK_SHADER_STAGE_COMPUTE_BIT,
                    m_shader)) {
    return false;
  }

  if (!attachmentSetup()) {
    return false;
  }

  if (!createPipeline()) {
    return false;
  }

  return true;
}

bool ComputePipeline::createPipeline() {
  if (!m_shader.valid()) {
    return false;
  }

  VkComputePipelineCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  createInfo.flags = m_flags;

  createInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  createInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
  createInfo.stage.module = m_shader.handle();
  createInfo.stage.pName = m_shader.entryPoint().data();
  createInfo.stage.pSpecializationInfo =
      nullptr; // TODO: Support specialization info

  createInfo.layout = m_layout;

  createInfo.basePipelineHandle = VK_NULL_HANDLE;
  createInfo.basePipelineIndex = -1;

  if (vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &createInfo,
                               nullptr, &m_pipeline) != VK_SUCCESS) {
    LOGE("Failed to create compute pipeline")
    return false;
  }
  return true;
}

void ComputePipeline::dispatch(VkCommandBuffer cmd, uint32_t groupCountX,
                               uint32_t groupCountY,
                               uint32_t groupCountZ) const {
  vkCmdDispatch(cmd, groupCountX, groupCountY, groupCountZ);
}

void ComputePipeline::bind(VkCommandBuffer cmd) const {
  ASSERT(m_pipeline != VK_NULL_HANDLE)
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline);
}

void ComputePipeline::bindDescriptors(VkCommandBuffer cmd,
                                      uint32_t frameIndex) const {
  LOGD("A")
  if (!m_descriptorGroups.empty()) {
    LOGD("B")
    std::vector<VkDescriptorSet> sets;
    LOGD("C")
    sets.reserve(m_descriptorGroups.size());

    LOGD("D {}",frameIndex)
    for (const auto &[set, group] : m_descriptorGroups) {
      sets.push_back(group.sets[frameIndex].handle());
    }

    LOGD("E")
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_layout, 0,
                            static_cast<uint32_t>(sets.size()), sets.data(), 0,
                            nullptr);
  }
}

void ComputePipeline::bindAndDispatch(VkCommandBuffer cmd, uint32_t frameIndex,
                                      uint32_t groupCountX,
                                      uint32_t groupCountY,
                                      uint32_t groupCountZ) const {
  LOGD("BIND")
  bind(cmd);
  LOGD("BIND DESCRIPTORS")
  bindDescriptors(cmd, frameIndex);
  LOGD("DISPATCH")
  dispatch(cmd, groupCountX, groupCountY, groupCountZ);
}

void ComputePipeline::destroy() {
  m_shader.destroy();
  destroyBase();
}

} // namespace vvhl