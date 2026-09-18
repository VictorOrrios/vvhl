#pragma once

#include "Pipeline.hpp"
#include <vvhl/RenderPass/RenderPass.hpp>

namespace vvhl {

enum class GraphicsPipelineMode {
  Raster, // Vertex -> (Tess) -> (Geom) -> Fragment
  Mesh    // (Task) -> Mesh -> Fragment
};

// Shader stages

struct RasterShaderStages {
  ShaderInput vertex = {""};
  ShaderInput fragment = {""};
  std::optional<ShaderInput> tessControl = std::nullopt;
  std::optional<ShaderInput> tessEval = std::nullopt;
  std::optional<ShaderInput> geometry = std::nullopt;
};

struct MeshShaderStages {
  ShaderInput mesh;
  ShaderInput fragment;
  std::optional<ShaderInput> task = std::nullopt;
};

// Obligatory, Raster mode only

struct VertexBinding {
  uint32_t binding = 0;
  uint32_t stride = 0;
  VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  [[nodiscard]] VkVertexInputBindingDescription toVkStruct() const {
    VkVertexInputBindingDescription desc{};
    desc.binding = binding;
    desc.stride = stride;
    desc.inputRate = inputRate;
    return desc;
  }
};

struct VertexAttribute {
  uint32_t location = 0;
  uint32_t binding = 0;
  VkFormat format = VK_FORMAT_R32G32B32_SFLOAT;
  uint32_t offset = 0;

  [[nodiscard]] VkVertexInputAttributeDescription toVkStruct() const {
    VkVertexInputAttributeDescription desc{};
    desc.location = location;
    desc.binding = binding;
    desc.format = format;
    desc.offset = offset;
    return desc;
  }
};

struct VertexInputState {
  std::vector<VertexBinding> bindings;
  std::vector<VertexAttribute> attributes;

  // Bundle to mantain in scope the arrays used to create the VkStruct
  struct VkBundle {
    VkPipelineVertexInputStateCreateInfo info{};
    std::vector<VkVertexInputBindingDescription> bindingDescs;
    std::vector<VkVertexInputAttributeDescription> attributeDescs;
  };

  [[nodiscard]] VkBundle toVkBundle() const {
    VkBundle bundle;

    bundle.bindingDescs.reserve(bindings.size());
    for (const auto &b : bindings)
      bundle.bindingDescs.push_back(b.toVkStruct());

    bundle.attributeDescs.reserve(attributes.size());
    for (const auto &a : attributes)
      bundle.attributeDescs.push_back(a.toVkStruct());

    bundle.info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    bundle.info.vertexBindingDescriptionCount =
        static_cast<uint32_t>(bundle.bindingDescs.size());
    bundle.info.pVertexBindingDescriptions = bundle.bindingDescs.data();
    bundle.info.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(bundle.attributeDescs.size());
    bundle.info.pVertexAttributeDescriptions = bundle.attributeDescs.data();

    return bundle;
  }
};

/*  Example:
struct Vertex {
  glm::vec3 position;  // offset 0,  size 12
  glm::vec3 normal;    // offset 12, size 12
  glm::vec2 uv;        // offset 24, size 8
};                     // stride = 32

info.vertexInput.bindings = {
  {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}
};
info.vertexInput.attributes = {
  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
  {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)},
  {2, 0, VK_FORMAT_R32G32_SFLOAT,    offsetof(Vertex, uv)},
};
*/

// INPUT ASSEMBLY

struct InputAssemblyState {
  VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  VkBool32 primitiveRestartEnable = VK_FALSE;

  [[nodiscard]] VkPipelineInputAssemblyStateCreateInfo toVkStruct() const {
    VkPipelineInputAssemblyStateCreateInfo info{};
    info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    info.topology = topology;
    info.primitiveRestartEnable = primitiveRestartEnable;
    return info;
  }
};

// RASTERIZATION

struct RasterizationState {
  VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
  VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
  VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  VkBool32 rasterizerDiscardEnable = VK_FALSE;
  VkBool32 depthClampEnable = VK_FALSE;
  VkBool32 depthBiasEnable = VK_FALSE;
  float depthBiasConstantFactor = 0.0f;
  float depthBiasClamp = 0.0f;
  float depthBiasSlopeFactor = 0.0f;
  float lineWidth = 1.0f;

  [[nodiscard]] VkPipelineRasterizationStateCreateInfo toVkStruct() const {
    VkPipelineRasterizationStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    info.polygonMode = polygonMode;
    info.cullMode = cullMode;
    info.frontFace = frontFace;
    info.rasterizerDiscardEnable = rasterizerDiscardEnable;
    info.depthClampEnable = depthClampEnable;
    info.depthBiasEnable = depthBiasEnable;
    info.depthBiasConstantFactor = depthBiasConstantFactor;
    info.depthBiasClamp = depthBiasClamp;
    info.depthBiasSlopeFactor = depthBiasSlopeFactor;
    info.lineWidth = lineWidth;
    return info;
  }
};

// TESSELLATION (optional, only raster)

struct TessellationState {
  uint32_t patchControlPoints = 0; // 0 = no tessellation

  [[nodiscard]] VkPipelineTessellationStateCreateInfo toVkStruct() const {
    VkPipelineTessellationStateCreateInfo info{};
    info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    info.patchControlPoints = patchControlPoints;
    return info;
  }

  [[nodiscard]] bool isEnabled() const { return patchControlPoints > 0; }
};

// VIEWPORT

// Viewport and scissor management WILL be dynamic states
struct ViewportState {
  uint32_t viewportCount = 1;
  uint32_t scissorCount = 1;

  [[nodiscard]] VkPipelineViewportStateCreateInfo toVkStruct() const {
    VkPipelineViewportStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    info.viewportCount = viewportCount;
    info.scissorCount = scissorCount;
    return info;
  }
};

// MULTISAMPLE

struct MultisampleState {
  VkSampleCountFlagBits rasterizationSamples =
      VK_SAMPLE_COUNT_1_BIT; // 1 Bit to turn off MSAA
  VkBool32 sampleShadingEnable = VK_FALSE;
  float minSampleShading = 1.0f;
  const VkSampleMask *sampleMask = nullptr;
  VkBool32 alphaToCoverageEnable = VK_FALSE;
  VkBool32 alphaToOneEnable = VK_FALSE;

  [[nodiscard]] VkPipelineMultisampleStateCreateInfo toVkStruct() const {
    VkPipelineMultisampleStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    info.rasterizationSamples = rasterizationSamples;
    info.sampleShadingEnable = sampleShadingEnable;
    info.minSampleShading = minSampleShading;
    info.pSampleMask = sampleMask;
    info.alphaToCoverageEnable = alphaToCoverageEnable;
    info.alphaToOneEnable = alphaToOneEnable;
    return info;
  }
};

// COLOR BLEND

struct ColorBlendAttachment {
  VkBool32 blendEnable = VK_FALSE;
  VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  VkBlendOp colorBlendOp = VK_BLEND_OP_ADD;
  VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD;
  VkColorComponentFlags colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  [[nodiscard]] VkPipelineColorBlendAttachmentState toVkStruct() const {
    VkPipelineColorBlendAttachmentState state{};
    state.blendEnable = blendEnable;
    state.srcColorBlendFactor = srcColorBlendFactor;
    state.dstColorBlendFactor = dstColorBlendFactor;
    state.colorBlendOp = colorBlendOp;
    state.srcAlphaBlendFactor = srcAlphaBlendFactor;
    state.dstAlphaBlendFactor = dstAlphaBlendFactor;
    state.alphaBlendOp = alphaBlendOp;
    state.colorWriteMask = colorWriteMask;
    return state;
  }
};

struct ColorBlendState {
  VkBool32 logicOpEnable = VK_FALSE;
  VkLogicOp logicOp = VK_LOGIC_OP_COPY;
  std::vector<ColorBlendAttachment> attachments;
  float blendConstants[4] = {0.0f, 0.0f, 0.0f, 0.0f};

  struct VkBundle {
    VkPipelineColorBlendStateCreateInfo info{};
    std::vector<VkPipelineColorBlendAttachmentState> attachmentStates;
  };

  [[nodiscard]] VkBundle toVkBundle() const {
    VkBundle bundle;

    bundle.attachmentStates.reserve(attachments.size());
    for (const auto &a : attachments)
      bundle.attachmentStates.push_back(a.toVkStruct());

    bundle.info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    bundle.info.logicOpEnable = logicOpEnable;
    bundle.info.logicOp = logicOp;
    bundle.info.attachmentCount =
        static_cast<uint32_t>(bundle.attachmentStates.size());
    bundle.info.pAttachments = bundle.attachmentStates.data();
    std::copy(std::begin(blendConstants), std::end(blendConstants),
              std::begin(bundle.info.blendConstants));

    return bundle;
  }
};

// DYNAMIC STATE

struct DynamicState {
  std::vector<VkDynamicState> states = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
  };

  [[nodiscard]] VkPipelineDynamicStateCreateInfo toVkStruct() const {
    VkPipelineDynamicStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    info.dynamicStateCount = static_cast<uint32_t>(states.size());
    info.pDynamicStates = states.data();
    return info;
  }

  [[nodiscard]] bool empty() const { return states.empty(); }
};

// RENDERING FORMATS (dynamic rendering)

struct RenderingFormats {
  std::vector<VkFormat> colorFormats;
  VkFormat depthFormat = VK_FORMAT_UNDEFINED;
  VkFormat stencilFormat = VK_FORMAT_UNDEFINED;
  uint32_t viewMask = 0;

  [[nodiscard]] VkPipelineRenderingCreateInfo toVkStruct() const {
    VkPipelineRenderingCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    info.colorAttachmentCount = static_cast<uint32_t>(colorFormats.size());
    info.pColorAttachmentFormats = colorFormats.data();
    info.depthAttachmentFormat = depthFormat;
    info.stencilAttachmentFormat = stencilFormat;
    info.viewMask = viewMask;
    return info;
  }
};

//  DEPTH / STENCIL (optional, common for both modes)

struct StencilOpState {
  VkStencilOp failOp = VK_STENCIL_OP_KEEP;
  VkStencilOp passOp = VK_STENCIL_OP_KEEP;
  VkStencilOp depthFailOp = VK_STENCIL_OP_KEEP;
  VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS;
  uint32_t compareMask = 0;
  uint32_t writeMask = 0;
  uint32_t reference = 0;

  [[nodiscard]] VkStencilOpState toVkStruct() const {
    VkStencilOpState state{};
    state.failOp = failOp;
    state.passOp = passOp;
    state.depthFailOp = depthFailOp;
    state.compareOp = compareOp;
    state.compareMask = compareMask;
    state.writeMask = writeMask;
    state.reference = reference;
    return state;
  }
};

struct DepthStencilState {
  VkBool32 depthTestEnable = VK_TRUE;
  VkBool32 depthWriteEnable = VK_TRUE;
  VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS;
  VkBool32 depthBoundsTestEnable = VK_FALSE;
  VkBool32 stencilTestEnable = VK_FALSE;
  StencilOpState front;
  StencilOpState back;
  float minDepthBounds = 0.0f;
  float maxDepthBounds = 1.0f;

  [[nodiscard]] VkPipelineDepthStencilStateCreateInfo toVkStruct() const {
    VkPipelineDepthStencilStateCreateInfo info{};
    info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    info.depthTestEnable = depthTestEnable;
    info.depthWriteEnable = depthWriteEnable;
    info.depthCompareOp = depthCompareOp;
    info.depthBoundsTestEnable = depthBoundsTestEnable;
    info.stencilTestEnable = stencilTestEnable;
    info.front = front.toVkStruct();
    info.back = back.toVkStruct();
    info.minDepthBounds = minDepthBounds;
    info.maxDepthBounds = maxDepthBounds;
    return info;
  }
};

// CREATE INFO

struct GraphicsPipelineCreateInfo {
  RenderPass *renderPass = nullptr;
  GraphicsPipelineMode mode = GraphicsPipelineMode::Raster;
  VkPipelineCreateFlags flags = 0;

  // Shaders. Only one will be read, based on the mode
  RasterShaderStages rasterShaders = {};
  MeshShaderStages meshShaders = {};

  // Common
  ViewportState viewport = {};
  RasterizationState rasterization = {};
  MultisampleState multisample = {};
  DepthStencilState depthStencil = {}; // Optional
  ColorBlendState colorBlend = {};
  DynamicState dynamicState = {};
  RenderingFormats formats = {};

  // Only raster mode
  VertexInputState vertexInput = {};
  InputAssemblyState inputAssembly = {};
  TessellationState tessellation = {}; // Optional
};


class GraphicsPipeline : public Pipeline {
public:
  struct CreateInfo {
    RenderPass *renderPass;
    ShaderInput shaderInput;
    VkPipelineCreateFlags flags = 0;
  };

public:
  GraphicsPipeline() = default;
  ~GraphicsPipeline() = default;

  GraphicsPipeline(const GraphicsPipeline &) = delete;
  GraphicsPipeline &operator=(const GraphicsPipeline &) = delete;

  bool initialize(const GraphicsPipelineCreateInfo &createInfo);

  virtual void destroy() override;

  // Draw
  void draw(VkCommandBuffer cmd, uint32_t vertexCount,
            uint32_t instanceCount = 1, uint32_t firstVertex = 0,
            uint32_t firstInstance = 0) const;

  void drawIndexed(VkCommandBuffer cmd, uint32_t indexCount,
                   uint32_t instanceCount = 1, uint32_t firstIndex = 0,
                   int32_t vertexOffset = 0, uint32_t firstInstance = 0) const;

  void drawMeshTasks(VkCommandBuffer cmd, uint32_t groupCountX,
                     uint32_t groupCountY, uint32_t groupCountZ) const;

public:
  GraphicsPipelineMode mode() const { return m_mode; }

private:
  bool createPipeline() override;
  bool buildShaderStages();
  bool createRasterPipeline();
  bool createMeshPipeline();

  void createShaderStage(VkShaderStageFlagBits stage,
  ShaderInput input);

private:
  GraphicsPipelineMode m_mode = GraphicsPipelineMode::Raster;
  VkPipelineCreateFlags m_flags = 0;

  // Shaders
  std::deque<Shader> m_shaders;
  std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

  GraphicsPipelineCreateInfo m_createInfo;
};

} // namespace vvhl