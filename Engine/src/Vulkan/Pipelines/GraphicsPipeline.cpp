#include <vulkan/vulkan_core.h>
#include <vvhl/Vulkan/Pipelines/GraphicsPipeline.hpp>

namespace vvhl {

bool GraphicsPipeline::initialize(
    const GraphicsPipelineCreateInfo &createInfo) {
  destroy();

  m_bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  if (createInfo.renderPass == nullptr ||
      createInfo.renderPass->context().deviceHandle() == VK_NULL_HANDLE) {
    LOGE("Pipeline created with empty renderpass reference")
    return false;
  }

  m_device = createInfo.renderPass->context().deviceHandle();
  m_flags = createInfo.flags;
  m_pool = &createInfo.renderPass->descriptorPool();
  m_resourceManager = &createInfo.renderPass->resourceManager();

  // Set mode: Raster or Mesh
  m_mode = createInfo.mode;

  // Save the createInfo
  m_createInfo = createInfo;

  // TODO: Create shaders

  if (!attachmentSetup()) {
    return false;
  }

  if (!buildShaderStages()) {
    return false;
  }

  if (!createPipeline()) {
    return false;
  }

  return true;
}

void GraphicsPipeline::createShaderStage(VkShaderStageFlagBits stage,
                                         ShaderInput input) {
  m_shaders.push_back({});
  createShader(input, stage, m_shaders.back());
  m_shaderStages.push_back(
      {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
       .pNext = nullptr,
       .flags = 0,
       .stage = stage,
       .module = m_shaders.back().handle(),
       .pName = m_shaders.back().entryPoint().c_str(),
       .pSpecializationInfo = nullptr});
}

bool GraphicsPipeline::buildShaderStages() {
  if (m_mode == GraphicsPipelineMode::Raster) {
    // VERTEX
    createShaderStage(VK_SHADER_STAGE_VERTEX_BIT,
                      m_createInfo.rasterShaders.vertex);

    // TESSELATION
    if (m_createInfo.rasterShaders.tessControl.has_value() ^
        m_createInfo.rasterShaders.tessEval.has_value()) {
      LOGE("Only one of the tesselation shaders is filled with and input")
      return false;
    }
    if (m_createInfo.rasterShaders.tessControl.has_value() &&
        m_createInfo.rasterShaders.tessEval.has_value()) {
      createShaderStage(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
                        m_createInfo.rasterShaders.tessControl.value());
      createShaderStage(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
                        m_createInfo.rasterShaders.tessEval.value());
    }

    // GEOMETRY
    if (m_createInfo.rasterShaders.geometry.has_value()) {
      createShaderStage(VK_SHADER_STAGE_GEOMETRY_BIT,
                        m_createInfo.rasterShaders.geometry.value());
    }

    // FRAGMENT
    createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT,
                      m_createInfo.rasterShaders.fragment);
  } else {
    // TASK
    if (m_createInfo.meshShaders.task.has_value()) {
      createShaderStage(VK_SHADER_STAGE_TASK_BIT_EXT,
                        m_createInfo.meshShaders.task.value());
    }

    // MESH
    createShaderStage(VK_SHADER_STAGE_MESH_BIT_EXT,
                      m_createInfo.meshShaders.mesh);

    // FRAGMENT
    createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT,
                      m_createInfo.meshShaders.fragment);
  }
  return true;
}

bool GraphicsPipeline::createPipeline() {
  // Checks
  if (m_createInfo.dynamicState.empty()) {
    LOGE("Dynamic state of graphics pipeline has to have at least the viewport "
         "and scissor")
    return false;
  }
  if ((m_createInfo.depthStencil.depthTestEnable == VK_TRUE) ^
      (m_createInfo.formats.depthFormat != VK_FORMAT_UNDEFINED)) {
    LOGW("Graphics pipeline: using the depth buffer requieres defining a "
         "formats.depthFormat and enabling depthStencil.depthTestEnable")
  }

  if (m_mode == GraphicsPipelineMode::Raster) {
    return createRasterPipeline();
  }
  return createMeshPipeline();
}

bool GraphicsPipeline::createRasterPipeline() {
  // 1. Convert to vk structs
  auto vertexInput = m_createInfo.vertexInput.toVkBundle();
  auto inputAssembly = m_createInfo.inputAssembly.toVkStruct();
  auto tessellation = m_createInfo.tessellation.toVkStruct();
  auto viewport = m_createInfo.viewport.toVkStruct();
  auto rasterization = m_createInfo.rasterization.toVkStruct();
  auto multisample = m_createInfo.multisample.toVkStruct();
  auto depthStencil = m_createInfo.depthStencil.toVkStruct();
  auto colorBlend = m_createInfo.colorBlend.toVkBundle();
  auto dynamic = m_createInfo.dynamicState.toVkStruct();
  auto rendering = m_createInfo.formats.toVkStruct();

  // 2. Assemble the create struct
  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pNext = &rendering;
  pipelineInfo.flags = m_flags;

  // Shaders
  pipelineInfo.stageCount = static_cast<uint32_t>(m_shaderStages.size());
  pipelineInfo.pStages = m_shaderStages.data();

  // States
  pipelineInfo.pVertexInputState = &vertexInput.info;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pTessellationState =
      m_createInfo.tessellation.isEnabled() ? &tessellation : nullptr;
  pipelineInfo.pViewportState = &viewport;
  pipelineInfo.pRasterizationState = &rasterization;
  pipelineInfo.pMultisampleState = &multisample;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlend.info;
  pipelineInfo.pDynamicState =
      m_createInfo.dynamicState.empty() ? nullptr : &dynamic;

  // Layout
  pipelineInfo.layout = m_layout;

  // Dynamic rendering
  pipelineInfo.renderPass = VK_NULL_HANDLE;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex = -1;

  // 3. Create pipeline
  if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                nullptr, &m_pipeline) != VK_SUCCESS) {
    LOGE("Failed to create raster graphics pipeline")
    return false;
  }

  return true;
}

bool GraphicsPipeline::createMeshPipeline() {

  // 1. Convert to vk structs
  auto viewport = m_createInfo.viewport.toVkStruct();
  auto rasterization = m_createInfo.rasterization.toVkStruct();
  auto multisample = m_createInfo.multisample.toVkStruct();
  auto depthStencil = m_createInfo.depthStencil.toVkStruct();
  auto colorBlend = m_createInfo.colorBlend.toVkBundle();
  auto dynamic = m_createInfo.dynamicState.toVkStruct();
  auto rendering = m_createInfo.formats.toVkStruct();

  // 2. Assemble the create struct
  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pNext = &rendering;
  pipelineInfo.flags = m_flags;

  // Shaders
  pipelineInfo.stageCount = static_cast<uint32_t>(m_shaderStages.size());
  pipelineInfo.pStages = m_shaderStages.data();

  // Raster states (all nullptr)
  pipelineInfo.pVertexInputState = nullptr;
  pipelineInfo.pInputAssemblyState = nullptr;
  pipelineInfo.pTessellationState = nullptr;

  // Common states
  pipelineInfo.pViewportState = &viewport;
  pipelineInfo.pRasterizationState = &rasterization;
  pipelineInfo.pMultisampleState = &multisample;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlend.info;
  pipelineInfo.pDynamicState =
      m_createInfo.dynamicState.empty() ? nullptr : &dynamic;

  // Layout
  pipelineInfo.layout = m_layout;

  // Dynamic rendering
  pipelineInfo.renderPass = VK_NULL_HANDLE;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex = -1;

  // 3. Create pipeline
  if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                nullptr, &m_pipeline) != VK_SUCCESS) {
    LOGE("Failed to create mesh graphics pipeline")
    return false;
  }

  return true;
}

void GraphicsPipeline::draw(VkCommandBuffer cmd, uint32_t vertexCount,
                            uint32_t instanceCount, uint32_t firstVertex,
                            uint32_t firstInstance) const {

  vkCmdDraw(cmd, vertexCount, instanceCount, firstVertex, firstInstance);
}

void GraphicsPipeline::drawIndexed(VkCommandBuffer cmd, uint32_t indexCount,
                                   uint32_t instanceCount, uint32_t firstIndex,
                                   int32_t vertexOffset,
                                   uint32_t firstInstance) const {
  vkCmdDrawIndexed(cmd, indexCount, instanceCount, firstIndex, vertexOffset,
                   firstInstance);
}

void GraphicsPipeline::drawMeshTasks(VkCommandBuffer cmd, uint32_t groupCountX,
                                     uint32_t groupCountY,
                                     uint32_t groupCountZ) const {
  vkCmdDrawMeshTasksEXT(cmd, groupCountX, groupCountY, groupCountZ);
}

void GraphicsPipeline::destroy() {
  for (auto &shader : m_shaders)
    shader.destroy();
  destroyBase();
}

} // namespace vvhl
