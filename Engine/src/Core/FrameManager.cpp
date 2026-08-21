#include "vvhl/Vulkan/Context/Device.hpp"
#include "vvhl/Vulkan/Sync/Barriermanager.hpp"
#include <vulkan/vulkan_core.h>
#include <vvhl/Core/FrameManager.hpp>

namespace vvhl {

bool FrameManager::initialize(VulkanContext &context, CommandPool &cmdPool) {

  bool success = true;

  m_frames.reserve(m_maxFramesInFlight);
  for (uint32_t i = 0; i < m_maxFramesInFlight; i++) {
    Frame f = {.frameNumber = i,
               .fence = Fence(),
               .imageAvailable = Semaphore(),
               .cmdBuffer = cmdPool.allocate()};

    success &= f.fence.initialize(context.device(), true);
    success &= f.imageAvailable.initialize(context.device());

    m_frames.push_back(std::move(f));
  }

  m_context = &context;
  m_cmdPool = &cmdPool;

  if (!success)
    destroy();

  return success;
}

void FrameManager::destroy() {
  for (auto &frame : m_frames) {
    frame.fence.destroy();
    frame.imageAvailable.destroy();
  }
  m_barrierManager.destroy();
  m_frames.clear();
  m_context = nullptr;
  m_cmdPool = nullptr;
  m_currentFrame = 0;
}

bool FrameManager::beginFrame(Frame *&currentFrame, VkImageView &outputView) {
  ASSERT(m_context != nullptr)

  currentFrame = &m_frames[m_currentFrame];

  ASSERT(m_currentFrame < m_maxFramesInFlight)
  ASSERT(currentFrame->frameNumber < m_maxFramesInFlight)

  // Wait for main fence
  if (currentFrame->fence.wait() != VK_SUCCESS) {
    LOGE("Error waiting at main frame fence")
    return false;
  }

  // Reset main fence
  if (currentFrame->fence.reset() != VK_SUCCESS) {
    LOGE("Error reseting main frame fence")
    return false;
  }

  // Reset current command buffer
  if (!currentFrame->cmdBuffer.reset())
    return false;

  // Bengin command buffer
  if (!currentFrame->cmdBuffer.begin())
    return false;

  // Acquire next swapchain image (output)
  if (m_context->swapchain().advanceImage(currentFrame->imageAvailable.handle(),
                                          VK_NULL_HANDLE) != VK_SUCCESS) {
    LOGE("Error acquiring next swapchain image")
    return false;
  }

  // Transition swapchain target to color attachment layout
  m_barrierManager.imageBarrier(m_context->swapchain().imageWrap())
      ->toLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
      ->stage(VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,            // srcStage
              VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT // dstStage
              )
      ->access(0,                                     // srcAccess
               VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT // dstAccess
      );
  m_barrierManager.submit(currentFrame->cmdBuffer.handle());

  outputView = m_context->swapchain().imageView();

  return true;
}

bool FrameManager::endFrame() {
  ASSERT(m_context != nullptr)
  auto &f = m_frames[m_currentFrame];

  // Create submit info
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore waitSemaphores[] = {f.imageAvailable.handle()};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  VkCommandBuffer cmdBuffers[] = {f.cmdBuffer.handle()};
  submitInfo.pCommandBuffers = cmdBuffers;
  submitInfo.commandBufferCount = 1;

  VkSemaphore signalSemaphores[] = {
      m_context->swapchain().semaphore().handle()};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  // Transition swapchain target to present layout
  m_barrierManager.imageBarrier(m_context->swapchain().imageWrap())
      ->toLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
      ->stage(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, // srcStage
              VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT           // dstStage
              )
      ->access(VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, // srcAccess
               0                                       // dstAccess
      );
  m_barrierManager.submit(f.cmdBuffer.handle());

  // End command buffer
  if (!f.cmdBuffer.end())
    return false;

  // Submit to queue
  if (!m_context->device().graphicsQueue().submit(submitInfo, f.fence.handle()))
    return false;

  // Present to swapchain
  if (m_context->swapchain().presentImage(
          m_context->swapchain().semaphore().handle()) != VK_SUCCESS) {
    LOGE("Error presenting swapchain image")
    return false;
  }

  // Advance frame counter
  m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;

  return true;
}

} // namespace vvhl