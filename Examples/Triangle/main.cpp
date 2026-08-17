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
    m_pipeline.writeAllFrames<ImageWriteDescriptor>(std::pair(0,0),
                                                    {.handle = m_mainImage});
    m_pipeline.updateDescriptors();

    // CONFIG
    VkExtent3D extent3d = m_resourceManager->image(m_mainImage).extent();
    VkExtent2D extent2d(extent3d.width, extent3d.height);
    RenderingConfig rConf = RenderingConfig::singleColor(
        m_resourceManager->image(m_mainImage).view(), extent2d);
    m_renderer.updateRenderingInfo(rConf);

    return true;
  }

  void destroy() override { destroyBase(); }

  void onRender(VkCommandBuffer cmd, uint32_t) override {

    m_renderer.begin(cmd);

    VkExtent3D extent3d = m_resourceManager->image(m_mainImage).extent();
    VkExtent2D extent2d(extent3d.width, extent3d.height);
    VkExtent2D groupCount = calcGroupCounts(extent2d, 16);
    m_pipeline.dispatch(cmd, groupCount.width, groupCount.height, 1);

    m_renderer.end(cmd);
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
}