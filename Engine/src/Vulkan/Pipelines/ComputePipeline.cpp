
#include "ComputePipeline.hpp"

namespace vvhl {
bool ComputePipeline::initialize(const CreateInfo &createInfo) {
  destroy();

  if (!createInfo.device) {
    return false;
  }

  m_device = createInfo.device;
  m_flags = createInfo.flags;

  if (!createShader(createInfo.shaderInput, createInfo.entryPoint,
                    VK_SHADER_STAGE_COMPUTE_BIT, m_shader)) {
    return false;
  }

  if (!attachmentSetup()) {
    return false;
  }

  if (createInfo.descriptorPool) {
    accumulatePoolResources(*createInfo.descriptorPool);
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
  createInfo.stage.pSpecializationInfo = nullptr; // TODO: Support specialization info

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

} // namespace vvhl