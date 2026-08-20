// Triangle example for vvhl

#include "vvhl/Core/App.hpp"
#include "vvhl/Core/EngineConfig.hpp"
#include "vvhl/Resources/ResourceManager.hpp"
#include "vvhl/Vulkan/Renderer/DynamicRenderer.hpp"
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

  void destroy() override { destroyBase(); }

  void onRender(VkCommandBuffer cmd, uint32_t frameIndex) override {
    LOGD("Renderpass on render")

    // m_renderer.begin(cmd);

    m_barriers.imageBarrier(m_mainImage)->toLayout(VK_IMAGE_LAYOUT_GENERAL);
    m_barriers.submit(cmd);

    VkExtent2D groupCount =
        calcGroupCounts(m_resourceManager->image(m_mainImage).extent2D(), 16);
    m_pipeline.bindAndDispatch(cmd, frameIndex, groupCount.width,
                               groupCount.height, 1);

    m_barriers.imageBarrier(m_mainImage)
        ->toLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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
    LOGD("App on render")
    m_pass.onRender(cmd, currentFrame);
  }

private:
  TrianglePass m_pass;
};

int main() {
  EngineSettings::initialize({});

  TriangleApp app;

  app.initialize({.windowSpec = {}});

  LOGD("App init done")
  app.run();

  app.destroy();
}