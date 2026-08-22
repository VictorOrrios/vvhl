// Triangle example for vvhl

#include "vvhl/Core/App.hpp"
#include "vvhl/Core/EngineConfig.hpp"
#include "vvhl/Resources/ResourceManager.hpp"
#include "vvhl/Vulkan/Renderer/DynamicRenderer.hpp"
#include <vulkan/vulkan_core.h>
#include <vvhl/RenderPass/RenderPass.hpp>
#include <vvhl/Vulkan/Pipelines/ComputePipeline.hpp>
#include <vvhl/vvhl.hpp>

using namespace vvhl;

class TrianglePass : public RenderPass {
public:
  struct TrianglePassInput {
    App *app;
    ImageHandle mainInputOutput;
  };

public:
  bool initialize(TrianglePassInput input) {
    initializeBase(*input.app);

    // INPUT
    m_mainImage = input.mainInputOutput;
    m_descPool.accumulate({VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1});
    m_descPool.accumulateSet(1);

    // OUTPUT

    // DESCRIPTOR POOL
    m_descPool.create(m_context->device().handle());

    // PIPELINES
    m_pipeline.initialize({
        .renderPass = this,
        .shaderInput = {"./Examples/Triangle/triangle.slang"},
    });

    // DESCRIPTORS
    m_pipeline.writeAllFrames<ImageWriteDescriptor>(
        std::pair(0, 0),
        {.handle = m_mainImage, .imageLayout = VK_IMAGE_LAYOUT_GENERAL});
    m_pipeline.updateDescriptors();

    // CONFIG
    VkExtent2D extent2d = m_resourceManager->image(m_mainImage).extent2D();
    RenderingConfig rConf = RenderingConfig::singleColor(
        m_resourceManager->image(m_mainImage).view(), extent2d);
    m_renderer.updateRenderingInfo(rConf);

    return true;
  }

  void destroy() override {
    m_pipeline.destroy();
    destroyBase();
  }

  void onRender(VkCommandBuffer cmd, uint32_t frameIndex) override {

    // m_renderer.begin(cmd);

    m_barriers.imageBarrier(m_mainImage)
        ->toLayout(VK_IMAGE_LAYOUT_GENERAL)
        ->stage(VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT)
        ->access(0, VK_ACCESS_2_SHADER_WRITE_BIT);
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

    VkExtent2D groupCount =
        calcGroupCounts(m_resourceManager->image(m_mainImage).extent2D(), 1);
    m_pipeline.bindAndDispatch(cmd, frameIndex, groupCount.width,
                               groupCount.height, 1);

    m_barriers.imageBarrier(m_mainImage)
        ->toLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        ->stage(VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
                // VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT
                VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT)
        ->access(VK_ACCESS_2_SHADER_WRITE_BIT, VK_ACCESS_2_SHADER_READ_BIT);
    m_barriers.submit(cmd);

    // m_renderer.end(cmd);
  }

private:
  ComputePipeline m_pipeline;
  ImageHandle m_mainImage;
};

class TriangleApp : public App {
public:
  void initialize(AppConfig config) {
    initializeBase(config);
    m_pass.initialize({.app = this, .mainInputOutput = m_viewport});
  }

  void destroy() override {
    m_pass.destroy();
    destroyBase();
  }

  void onRender(VkCommandBuffer cmd, uint32_t currentFrame) override {
    m_pass.onRender(cmd, currentFrame);
  }

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