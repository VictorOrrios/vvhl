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
  ShaderInput vertex;
  ShaderInput fragment;
  std::optional<ShaderInput> tessControl;
  std::optional<ShaderInput> tessEval;
  std::optional<ShaderInput> geometry;
};

struct MeshShaderStages {
  ShaderInput mesh;
  ShaderInput fragment;
  std::optional<ShaderInput> task;
};

// Obligatory, Raster mode only

struct InputAssemblyState {
  VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  VkBool32 primitiveRestartEnable = VK_FALSE;
};

struct VertexBinding {
  uint32_t binding = 0;
  uint32_t stride = 0;
  VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
};

struct VertexAttribute {
  uint32_t location = 0;
  uint32_t binding = 0;
  VkFormat format = VK_FORMAT_R32G32B32_SFLOAT;
  uint32_t offset = 0;
};

struct VertexInputState {
  std::vector<VertexBinding> bindings;
  std::vector<VertexAttribute> attributes;
};

/*  Example:
struct Vertex {
  glm::vec3 position;  // offset 0,  size 12
  glm::vec3 normal;    // offset 12, size 12
  glm::vec2 uv;        // offset 24, size 8
};                       // stride = 32

info.vertexInput.bindings = {
  {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}
};
info.vertexInput.attributes = {
  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
  {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)},
  {2, 0, VK_FORMAT_R32G32_SFLOAT,    offsetof(Vertex, uv)},
};
*/

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
};

// Optional, Raster mode only

struct TessellationState {
  uint32_t patchControlPoints = 0; // 0 = no tessellation
};

// Common (Both modes)

struct ViewportState {
  uint32_t viewportCount = 1;
  uint32_t scissorCount = 1;
};

struct MultisampleState {
  VkSampleCountFlagBits rasterizationSamples =
      VK_SAMPLE_COUNT_1_BIT; // 1 Bit to turn off MSAA
  VkBool32 sampleShadingEnable = VK_FALSE;
  float minSampleShading = 1.0f;
  const VkSampleMask *sampleMask = nullptr;
  VkBool32 alphaToCoverageEnable = VK_FALSE;
  VkBool32 alphaToOneEnable = VK_FALSE;
};

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
};

struct ColorBlendState {
  VkBool32 logicOpEnable = VK_FALSE;
  VkLogicOp logicOp = VK_LOGIC_OP_COPY;
  std::vector<ColorBlendAttachment> attachments;
  float blendConstants[4] = {0.0f, 0.0f, 0.0f, 0.0f};
};

struct DynamicState {
  std::vector<VkDynamicState> states;
};

struct RenderingFormats {
  std::vector<VkFormat> colorFormats;
  VkFormat depthFormat = VK_FORMAT_UNDEFINED;
  VkFormat stencilFormat = VK_FORMAT_UNDEFINED;
  uint32_t viewMask = 0;
};

// Optional common (Both modes)

struct StencilOpState {
  VkStencilOp failOp = VK_STENCIL_OP_KEEP;
  VkStencilOp passOp = VK_STENCIL_OP_KEEP;
  VkStencilOp depthFailOp = VK_STENCIL_OP_KEEP;
  VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS;
  uint32_t compareMask = 0;
  uint32_t writeMask = 0;
  uint32_t reference = 0;
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
};

// Create info
struct GraphicsPipelineCreateInfo {
  RenderPass *renderPass = nullptr;
  GraphicsPipelineMode mode = GraphicsPipelineMode::Raster;
  VkPipelineCreateFlags flags = 0;

  // Shaders. Only one will be read, based on the mode
  RasterShaderStages rasterShaders;
  MeshShaderStages meshShaders;

  // Common
  ViewportState viewport;
  RasterizationState rasterization;
  MultisampleState multisample;
  DepthStencilState depthStencil; // Optional
  ColorBlendState colorBlend;
  DynamicState dynamicState;
  RenderingFormats formats;

  // Only raster mode
  VertexInputState vertexInput;
  InputAssemblyState inputAssembly;
  TessellationState tessellation; // Optional
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

  // Bind
  void bind(VkCommandBuffer cmd) const;
  void bindDescriptors(VkCommandBuffer cmd, uint32_t frameIndex) const;

  // Draw (según modo)
  void draw(VkCommandBuffer cmd, uint32_t vertexCount,
            uint32_t instanceCount = 1, uint32_t firstVertex = 0,
            uint32_t firstInstance = 0) const;

  void drawIndexed(VkCommandBuffer cmd, uint32_t indexCount,
                   uint32_t instanceCount = 1, uint32_t firstIndex = 0,
                   int32_t vertexOffset = 0, uint32_t firstInstance = 0) const;

  void drawMeshTasks(VkCommandBuffer cmd, uint32_t groupCountX,
                     uint32_t groupCountY, uint32_t groupCountZ) const;

  GraphicsPipelineMode mode() const { return m_mode; }

protected:
  bool createPipeline() override;

private:
  bool buildShaderStages();
  bool createRasterPipeline();
  bool createMeshPipeline();

private:
  GraphicsPipelineMode m_mode = GraphicsPipelineMode::Raster;
  VkPipelineCreateFlags m_flags = 0;

  // Shaders
  std::vector<Shader> m_shaders;
  std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

  // Estados (copias)
  VertexInputState m_vertexInput;
  InputAssemblyState m_inputAssembly;
  TessellationState m_tessellation;
  ViewportState m_viewport;
  RasterizationState m_rasterization;
  MultisampleState m_multisample;
  DepthStencilState m_depthStencil;
  ColorBlendState m_colorBlend;
  DynamicState m_dynamicState;
  RenderingFormats m_formats;
};

} // namespace vvhl