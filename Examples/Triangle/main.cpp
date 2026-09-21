// Triangle example for vvhl

#include "glm/fwd.hpp"
#include "vvhl/Core/App.hpp"
#include "vvhl/Core/EngineConfig.hpp"
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
  struct TrianglePassInput {
    App *app;
    GBufferID mainInputOutput;
  };

public:
  bool initialize(TrianglePassInput input) {
    initializeBase(*input.app);
    m_name = "Triangle pass";

    // INPUT
    m_mainImage = m_gbuffers->get(input.mainInputOutput);
    m_descPool.accumulate({VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1});
    m_descPool.accumulateSet(1);

    std::vector<glm::vec2> vertices = {
        glm::vec2(-0.5, 0.5),
        glm::vec2(0.5, 0.5),
        glm::vec2(0.0, -0.5),
    };

    BufferCreateDescription vtxBufferInfo = BufferPresets::Vertex;
    vtxBufferInfo.size = vertices.size() * sizeof(glm::vec2);
    m_vertexBuffer = m_resourceManager->createBuffer(vtxBufferInfo);

    CommandBuffer tempCmd = m_cmdPool->beginTemp();
    m_resourceManager->buffer(m_vertexBuffer)
        .update(tempCmd.handle(), vertices);
    tempCmd.end();
    m_context->device().graphicsQueue().submit(tempCmd.handle());
    m_context->device().graphicsQueue().waitIdle();

    // OUTPUT

    // DESCRIPTOR POOL
    m_descPool.create(m_context->device().handle());

    // PIPELINES
    m_computePipeline.initialize({
        .renderPass = this,
        .shaderInput = {"./Examples/Triangle/triangle.slang"},
    });

    GraphicsPipelineCreateInfo createInfo = {
        .renderPass = this,
        .mode = GraphicsPipelineMode::Raster,

        .rasterShaders = {.vertex = {"./Examples/Triangle/vertex.slang"},
                          .fragment = {"./Examples/Triangle/fragment.slang"}},
    };

    createInfo.formats.colorFormats.push_back(
        m_resourceManager->image(m_mainImage).format());
    createInfo.colorBlend.attachments.push_back({});

    createInfo.vertexInput.attributes.push_back(
        {.format = VK_FORMAT_R32G32_SFLOAT});
    createInfo.vertexInput.bindings.push_back({.stride = sizeof(glm::vec2)});

    m_graphicsPipeline.initialize(createInfo);

    // DESCRIPTORS
    m_computePipeline.writeAllFrames<ImageWriteDescriptor>(
        "outImage",
        {.handle = m_mainImage, .imageLayout = VK_IMAGE_LAYOUT_GENERAL});
    m_computePipeline.updateDescriptors();

    // CONFIG
    VkExtent2D extent2d = m_resourceManager->image(m_mainImage).extent2D();
    RenderingConfig rConf = RenderingConfig::singleColor(
        m_resourceManager->image(m_mainImage).view(), extent2d);
    m_renderer.updateRenderingInfo(rConf);

    return true;
  }

  void destroy() override {
    m_computePipeline.destroy();
    m_graphicsPipeline.destroy();
    destroyBase();
  }

  void onRender(VkCommandBuffer cmd, uint32_t frameIndex) override {

    // VK_IMAGE_LAYOUT_GENERAL For compute
    // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL For graphics
    m_barriers.imageBarrier(m_mainImage)
        ->toLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        ->stage(VK_PIPELINE_STAGE_2_NONE,
                VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT)
        ->toAccess(VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);
    m_barriers.submit(cmd);

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

    m_renderer.begin(cmd);
    m_renderer.setViewportAndScissor(cmd);

    m_graphicsPipeline.bind(cmd, frameIndex);
    auto handle = m_resourceManager->buffer(m_vertexBuffer).handle();
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(
        cmd,
        0,
        1,
        &handle,
        &offset
    );
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
    VkExtent2D extent2d = m_resourceManager->image(m_mainImage).extent2D();
    RenderingConfig rConf = RenderingConfig::singleColor(
        m_resourceManager->image(m_mainImage).view(), extent2d);
    m_renderer.updateRenderingInfo(rConf);
  }

private:
  ComputePipeline m_computePipeline;
  GraphicsPipeline m_graphicsPipeline;
  ImageHandle m_mainImage;
  BufferHandle m_vertexBuffer;
};

class TriangleApp : public App {
  enum GBufferIds {
    RenderTarget = 0,
  };

public:
  void initialize(AppConfig config) {
    initializeBase(config);
    m_pass.initialize(
        {.app = this, .mainInputOutput = GBufferIds::RenderTarget});
  }

  void destroy() override {
    m_pass.destroy();
    destroyBase();
  }

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
