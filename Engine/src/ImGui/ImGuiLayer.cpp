
#include "vvhl/Core/EngineConfig.hpp"
#include "vvhl/Core/Logger.hpp"
#include <vvhl/ImGui/ImGuiLayer.hpp>

namespace vvhl {
  static PFN_vkVoidFunction ImGuiVulkanLoader(const char* function_name, void* user_data) {
    VkDevice device = static_cast<VkDevice>(user_data);
    LOGD("Loading function")
    return vkGetDeviceProcAddr(device, function_name);
}

bool ImGuiLayer::initialize(VulkanContext &context, Window &window) {
  m_context = &context;
  m_window = &window;
  m_device = context.device().handle();

  if (!createDescriptorPool()) {
    destroy();
    return false;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui::StyleColorsDark();

  float main_scale =
      ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
  ImGuiStyle &style = ImGui::GetStyle();
  style.ScaleAllSizes(main_scale);
  style.FontScaleDpi = main_scale;
  // style.FontSizeBase = 20.0f;
  // io.Fonts->AddFontDefaultVector();

  if (!ImGui_ImplGlfw_InitForVulkan(window.getNativeHandle(), true)) {
    LOGE("Failed to initialize GLFW backend");
    destroy();
    return false;
  }

  // Verificaciones
  ASSERT(context.instance() != VK_NULL_HANDLE)
  ASSERT(context.device().physicalHandle() != VK_NULL_HANDLE)
  ASSERT(m_device != VK_NULL_HANDLE)
  ASSERT(context.device().graphicsQueue().handle() != VK_NULL_HANDLE)
  ASSERT(m_descriptorPool.handle() != VK_NULL_HANDLE)
  LOGI("Queue family: {}", context.device().graphicsFamily())
  LOGI("MAX FIF: {}", EngineSettings::maxFramesInFlight())
  LOGI("Color format: {}",
       static_cast<int>(EngineSettings::get().swapchain.preferredFormat));

  // Verificar soporte de dynamic rendering
  VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};
  dynamicRenderingFeatures.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;

  VkPhysicalDeviceFeatures2 features2{};
  features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
  features2.pNext = &dynamicRenderingFeatures;

  vkGetPhysicalDeviceFeatures2(context.device().physicalHandle(), &features2);

  if (!dynamicRenderingFeatures.dynamicRendering) {
    LOGE("Dynamic rendering not supported!");
    return false;
  }

  ImGui_ImplVulkan_InitInfo initInfo = {};
  initInfo.ApiVersion = EngineSettings::get().instance.apiVersion;
  initInfo.Instance = context.instance();
  initInfo.PhysicalDevice = context.device().physicalHandle();
  initInfo.Device = context.device().handle();
  initInfo.QueueFamily = context.device().graphicsFamily();
  initInfo.Queue = context.device().graphicsQueue().handle();
  initInfo.DescriptorPool = m_descriptorPool.handle();
  initInfo.PipelineCache = VK_NULL_HANDLE;
  initInfo.MinImageCount = 2;
  initInfo.ImageCount = EngineSettings::maxFramesInFlight();
  initInfo.CheckVkResultFn = checkVkResult;
  initInfo.UseDynamicRendering = true;

  LOGD("PipelineInfoMain")
  initInfo.PipelineInfoMain = {};
  initInfo.PipelineInfoMain.RenderPass = VK_NULL_HANDLE;
  initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  initInfo.PipelineInfoMain.Subpass = 0;

  LOGD("dynamic")
  initInfo.PipelineInfoMain.PipelineRenderingCreateInfo = {};
  initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
  initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount =
      1;
  initInfo.PipelineInfoMain.PipelineRenderingCreateInfo
      .pColorAttachmentFormats =
      &EngineSettings::get().swapchain.preferredFormat;
  initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount =
      1;

  LOGD("Checking Vulkan functions...");
  LOGD("vkCmdBeginRendering: {}", (void *)vkCmdBeginRendering);
  LOGD("vkCmdEndRendering: {}", (void *)vkCmdEndRendering);
  LOGD("vkCmdBeginRenderingKHR: {}", (void *)vkCmdBeginRenderingKHR);
  LOGD("API Version: {}.{}.{}",
       VK_API_VERSION_MAJOR(EngineSettings::get().instance.apiVersion),
       VK_API_VERSION_MINOR(EngineSettings::get().instance.apiVersion),
       VK_API_VERSION_PATCH(EngineSettings::get().instance.apiVersion));

  VkDevice device = context.device().handle();
  LOGD("Device handle: {}", (void *)device);
  LOGD("Device is null: {}", device == VK_NULL_HANDLE);

  // Verificar que vkGetDeviceProcAddr funciona
  PFN_vkVoidFunction testFunc =
      vkGetDeviceProcAddr(device, "vkCmdBeginRendering");
  LOGD("Test vkGetDeviceProcAddr result: {}", (void *)testFunc);

  /*
  ImGui_ImplVulkan_LoadFunctions(
      EngineSettings::get().instance.apiVersion, // Versión de API
      [](const char *function_name, void *user_data) {
        VkDevice device = static_cast<VkDevice>(user_data);
        LOGD("Loaded function")
        return vkGetDeviceProcAddr(device, function_name);
      },
      (void *)context.device().handle() // user_data
  );
  */

  ImGui_ImplVulkan_LoadFunctions(
      EngineSettings::get().instance.apiVersion,
      ImGuiVulkanLoader,  // Función estática, no lambda
      (void*)context.device().handle()
  );


  LOGD("vulkan init")
  if (!ImGui_ImplVulkan_Init(&initInfo)) {
    LOGE("Failed initializing imgui vulkan implementation")
    // destroy();
    return false;
  }

  LOGD("END")

  return true;
}

bool ImGuiLayer::createDescriptorPool() {
  DescriptorPool::PoolSize pool_sizes[] = {
      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
       IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE + 10},
      {VK_DESCRIPTOR_TYPE_SAMPLER,
       IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE + 10},
  };

  m_descriptorPool.accumulate(pool_sizes);
  for (auto &pool_size : pool_sizes)
    m_descriptorPool.accumulateSet(pool_size.descriptorCount);
  m_descriptorPool.accumulateSet(5);
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