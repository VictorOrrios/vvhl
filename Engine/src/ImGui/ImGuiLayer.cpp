
#include "vvhl/Core/EngineConfig.hpp"
#include <vulkan/vulkan_core.h>
#include <vvhl/ImGui/ImGuiLayer.hpp>

namespace vvhl {

bool ImGuiLayer::initialize(VulkanContext &context, Window &window) {
  m_context = &context;
  m_window = &window;
  m_device = context.device().handle();

  // Create descriptor pool
  if (!createDescriptorPool()) {
    destroy();
    return false;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui::StyleColorsDark();

  // Setup scaling
  float main_scale =
      ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
  ImGuiStyle &style = ImGui::GetStyle();
  style.ScaleAllSizes(main_scale);
  style.FontScaleDpi = main_scale;
  style.FontSizeBase = 20.0f;
  io.Fonts->AddFontDefaultVector();

  // Setup vulkan backend
  ImGui_ImplGlfw_InitForVulkan(window.getNativeHandle(), true);

  ImGui_ImplVulkan_InitInfo initInfo = {};
  initInfo.ApiVersion = EngineSettings::get().instance.apiVersion;
  initInfo.Instance = context.instance();
  initInfo.PhysicalDevice = context.device().physicalHandle();
  initInfo.Device = m_device;
  initInfo.QueueFamily = context.device().graphicsFamily();
  initInfo.Queue = context.device().graphicsQueue().handle();
  initInfo.DescriptorPool = m_descriptorPool.handle();
  initInfo.MinImageCount = 2;
  initInfo.ImageCount = EngineSettings::maxFramesInFlight();

  // Configure dynamic rendering
  VkPipelineRenderingCreateInfo renderingInfo = {};
  renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
  renderingInfo.colorAttachmentCount = 1;
  VkFormat colorFormat = EngineSettings::get().swapchain.preferredFormat;
  renderingInfo.pColorAttachmentFormats = &colorFormat;
  initInfo.PipelineInfoMain.PipelineRenderingCreateInfo = renderingInfo;
  initInfo.PipelineInfoMain.RenderPass = VK_NULL_HANDLE;
  initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

  if (!ImGui_ImplVulkan_Init(&initInfo)) {
    destroy();
    return false;
  }

  return true;
}

bool ImGuiLayer::createDescriptorPool() {
  DescriptorPool::PoolSize pool_sizes[] = {
      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
       IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE},
      {VK_DESCRIPTOR_TYPE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE},
  };

  m_descriptorPool.accumulate(pool_sizes);
  if (!m_descriptorPool.create(
          m_device, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)) {
    LOGE("Failed creating ImGui Descriptor Pool")
    return false;
  }
  return true;
}

VkCommandBuffer ImGuiLayer::beginSingleTimeCommands(CommandPool &cmdPool) {
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = cmdPool.handle();
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer cmd;
  vkAllocateCommandBuffers(m_device, &allocInfo, &cmd);

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(cmd, &beginInfo);

  return cmd;
}

void ImGuiLayer::endSingleTimeCommands(CommandPool &cmdPool,
                                       VkCommandBuffer cmd) {
  vkEndCommandBuffer(cmd);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &cmd;

  vkQueueSubmit(m_context->device().graphicsQueue().handle(), 1, &submitInfo,
                VK_NULL_HANDLE);
  vkQueueWaitIdle(m_context->device().graphicsQueue().handle());

  vkFreeCommandBuffers(m_device, cmdPool.handle(), 1, &cmd);
}

void ImGuiLayer::beginFrame() {
  ASSERT(m_device != VK_NULL_HANDLE)

  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void ImGuiLayer::endFrame(VkCommandBuffer cmd, VkImageView targetView,
                          VkExtent2D extent) {
  ASSERT(m_device != VK_NULL_HANDLE)

  ImGui::Render();

  auto renderConfig = RenderingConfig::singleColor(targetView, extent);
  renderConfig.colorAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  m_renderer.updateRenderingInfo(renderConfig);

  m_renderer.begin(cmd);

  ImDrawData *drawData = ImGui::GetDrawData();
  ImGui_ImplVulkan_RenderDrawData(drawData, cmd);

  m_renderer.end(cmd);
}

void ImGuiLayer::destroy() {
  if (m_device == VK_NULL_HANDLE)
    return;

  if (m_device != VK_NULL_HANDLE) {
    vkDeviceWaitIdle(m_device);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    m_descriptorPool.destroy();
  }
}

VkExtent2D ImGuiLayer::getViewportExtent() const {
  ImGuiViewport *mainViewport = ImGui::GetMainViewport();

  VkExtent2D extent;
  extent.width = static_cast<uint32_t>(mainViewport->Size.x);
  extent.height = static_cast<uint32_t>(mainViewport->Size.y);

  return extent;
}

VkDescriptorSet ImGuiLayer::createViewportTextureId(VkImageView image,
                                                    VkSampler sampler) {
  return ImGui_ImplVulkan_AddTexture(sampler, image,
                                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

} // namespace vvhl