// Triangle example for vvhl

// TODO: Wrap everything into a vvhl.hpp, less imports
#include "glm/fwd.hpp"
#include "vvhl/Core/App.hpp"
#include "vvhl/Core/EngineConfig.hpp"
#include "vvhl/Core/EngineContext.hpp"
#include "vvhl/Resources/BufferPresets.hpp"
#include "vvhl/Resources/GBuffers.hpp"
#include "vvhl/Resources/ResourceManager.hpp"
#include "vvhl/Vulkan/Commands/CommandBuffer.hpp"
#include "vvhl/Vulkan/Memory/Buffer.hpp"
#include "vvhl/Vulkan/Renderer/DynamicRenderer.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>
#include <vvhl/RenderPass/RenderPass.hpp>
#include <vvhl/Vulkan/Pipelines/ComputePipeline.hpp>
#include <vvhl/Vulkan/Pipelines/GraphicsPipeline.hpp>
#include <vvhl/vvhl.hpp>

using namespace vvhl;

class TrianglePass : public RenderPass {
public:
  // TODO: Make initializacion more standar, maybe pass engine context as
  // argument
  struct TrianglePassInput {
    EngineContext &ctx;
    GBufferID mainInputOutput;
  };

public:
  bool initialize(TrianglePassInput input) {
    initializeBase(input.ctx); // TODO: This doesn't looks good

    // INPUT
    m_mainImage = m_ctx.gbuffers->get(input.mainInputOutput);
    // TODO: Descriptor pool accumulate doesn't look good.
    // Make pipelines do it? Research how big is a desc pool in memory
    m_descPool.accumulate({VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1});
    m_descPool.accumulateSet(1);

    std::vector<glm::vec2> vertices = {
        glm::vec2(-0.5, 0.5),
        glm::vec2(0.5, 0.5),
        glm::vec2(0.0, -0.5),
    };

    BufferCreateDescription vtxBufferInfo = BufferPresets::Vertex;
    vtxBufferInfo.size = vertices.size() * sizeof(glm::vec2);
    m_vertexBuffer = m_ctx.resourceManager->createBuffer(vtxBufferInfo);

    // TODO: Simplify. Maybe merge into a
    // m_ctx.cmdSystem.transfer.beginTempCmd()/.endTempCmd()
    CommandBuffer tempCmd = m_ctx.cmdSystem->transferPool().beginTemp();
    m_ctx.resourceManager->buffer(m_vertexBuffer)
        .update(tempCmd.handle(), vertices);
    tempCmd.end();
    m_ctx.cmdSystem->transferQueue().submitWait(tempCmd.handle());

    // OUTPUT

    // DESCRIPTOR POOL
    // TODO: Maybe skip this part, research other engines
    m_descPool.create(m_ctx.vkContext->device().handle());

    // PIPELINES
    // TODO: Create pipelines trough a renderpass manager/function?
    m_computePipeline.initialize({
        .renderPass = this,
        .shaderInput = {"./Examples/Triangle/triangle.slang"},
    });

    GraphicsPipelineCreateInfo createInfo = {
        .renderPass = this,
        .mode = GraphicsPipelineMode::Raster,

        .rasterShaders = {.vertex = {"./Examples/Triangle/vertex.slang"},
                          .fragment = {"./Examples/Triangle/fragment.slang"}},

        .colorBlend = {.attachments = {{}}},
        .formats = {.colorFormats =
                        {m_ctx.resourceManager->image(m_mainImage).format()}},
        .vertexInput = {
            .bindings = {{.stride = sizeof(glm::vec2)}},
            .attributes = {{.format = VK_FORMAT_R32G32_SFLOAT}},
        }};

    m_graphicsPipeline.initialize(createInfo);

    // DESCRIPTORS
    m_computePipeline.writeAllFrames<ImageWriteDescriptor>(
        "outImage",
        {.handle = m_mainImage, .imageLayout = VK_IMAGE_LAYOUT_GENERAL});
    m_computePipeline.updateDescriptors();

    // CONFIG
    // TODO: Pass resource manager to struct or renderer?
    VkExtent2D extent2d = m_ctx.resourceManager->image(m_mainImage).extent2D();
    RenderingConfig rConf = RenderingConfig::singleColor(
        m_ctx.resourceManager->image(m_mainImage).view(), extent2d);
    m_renderer.updateRenderingInfo(rConf);

    return true;
  }

  void destroy() override {
    // TODO: Register created pipelines trough manager/functions
    m_computePipeline.destroy();
    m_graphicsPipeline.destroy();
    destroyBase();
  }

  void onRender(VkCommandBuffer cmd, uint32_t frameIndex) override {

    // VK_IMAGE_LAYOUT_GENERAL For compute
    // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL For graphics
    // TODO: stage might be deducted trough layout and/or access
    m_barriers.imageBarrier(m_mainImage)
        ->toLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        ->stage(VK_PIPELINE_STAGE_2_NONE,
                VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT)
        ->toAccess(VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);
    m_barriers.submit(cmd);

    // TODO: Clear color helper
    /*
    VkClearColorValue color;
    color.float32[0] = 1.0f;
    color.float32[1] = 0.0f;
    color.float32[2] = 0.0f;
    color.float32[3] = 1.0f;
    VkImageSubresourceRange range;
    range.aspectMask =
        m_resourceManager->image(m_mainImage).description().aspectMask;
    range.baseArrayLayer =
        m_resourceManager->image(m_mainImage).syncState().baseArrayLayer;
    range.baseMipLevel =
        m_resourceManager->image(m_mainImage).syncState().baseMipLevel;
    range.layerCount =
        m_resourceManager->image(m_mainImage).syncState().arrayLayerCount;
    range.levelCount =
        m_resourceManager->image(m_mainImage).syncState().mipLevelCount;
    vkCmdClearColorImage(cmd, m_resourceManager->image(m_mainImage).handle(),
                         m_resourceManager->image(m_mainImage).layout(), &color,
                         1, &range);
    */

    /*
    VkExtent2D groupCount =
        calcGroupCounts(m_resourceManager->image(m_mainImage).extent2D(), 1);
    m_computePipeline.bindAndDispatch(cmd, frameIndex, groupCount.width,
                               groupCount.height, 1);
    */

    // Merge into one
    m_renderer.begin(cmd);
    m_renderer.setViewportAndScissor(cmd);

    // Merge bind and draw
    m_graphicsPipeline.bind(cmd, frameIndex);
    auto handle = m_ctx.resourceManager->buffer(m_vertexBuffer).handle();
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmd, 0, 1, &handle, &offset);
    m_graphicsPipeline.draw(cmd, 3);

    m_renderer.end(cmd);

    m_barriers.imageBarrier(m_mainImage)
        ->toLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        ->stage(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT)
        ->toAccess(VK_ACCESS_2_SHADER_READ_BIT);
    m_barriers.submit(cmd);
  }

  void onRenderGUI() override {
    if (ImGui::CollapsingHeader(m_name.c_str())) {
      ImGui::Text("Hola");
    }
  }

  void onViewportResize(const ViewportResizeEvent &) override {
    m_computePipeline.writeAllFrames<ImageWriteDescriptor>(
        "outImage",
        {.handle = m_mainImage, .imageLayout = VK_IMAGE_LAYOUT_GENERAL});
    m_computePipeline.updateDescriptors();
    VkExtent2D extent2d = m_ctx.resourceManager->image(m_mainImage).extent2D();
    RenderingConfig rConf = RenderingConfig::singleColor(
        m_ctx.resourceManager->image(m_mainImage).view(), extent2d);
    m_renderer.updateRenderingInfo(rConf);
  }

private:
  ComputePipeline m_computePipeline;
  GraphicsPipeline m_graphicsPipeline;
  ImageHandle m_mainImage;
  BufferHandle m_vertexBuffer;
  std::string m_name = "Triangle pass";
};

class TriangleApp : public App {
  // TODO: Reserving index 0 for render target might be bad
  enum GBufferIds {
    RenderTarget = 0,
  };

public:
  bool onAttach() override {
    auto ctx = makeEngineContext();
    return m_pass.initialize(
        {.ctx = ctx, .mainInputOutput = GBufferIds::RenderTarget});
  }

  void onDestroy() override { m_pass.destroy(); }

  void onRender(VkCommandBuffer cmd, uint32_t currentFrame) override {
    m_pass.onRender(cmd, currentFrame);
  }

  void onRenderGraphGUI() override { m_pass.onRenderGUI(); }

private:
  TrianglePass m_pass;
};

int main() {
  EngineSettings::initialize({});

  TriangleApp app;

  app.initialize({.windowSpec = {}});

  app.run();

  app.destroy();

  return 0;
}
