#include <vvhl/Core/FrameManager.hpp>

namespace vvhl {

bool FrameManager::initialize(VulkanContext &context, CommandPool &cmdPool) {

  bool success = true;

  m_frames.reserve(m_maxFramesInFlight);
  for (uint32_t i = 0; i < m_maxFramesInFlight; i++) {
    Frame f = {.frameNumber = i,
               .fence = Fence(),
               .imageAvailable = Semaphore(),
               .renderFinished = Semaphore(),
               .cmdBuffer = m_cmdPool->allocate()};

    success &= f.fence.initialize(context.device());
    success &= f.imageAvailable.initialize(context.device());
    success &= f.renderFinished.initialize(context.device());

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
    frame.renderFinished.destroy();
  }
  m_frames.clear();
  m_context = nullptr;
  m_cmdPool = nullptr;
  m_currentFrame = 0;
}

bool FrameManager::beginFrame(Frame *&currentFrame, VkImageView &outputView) {
  ASSERT(m_context != nullptr)

  currentFrame = &m_frames[m_currentFrame];

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
  if (m_context->swapchain().acquireNextImage(
          currentFrame->imageAvailable.handle(), VK_NULL_HANDLE,
          currentFrame->frameNumber) != VK_SUCCESS) {
    LOGE("Error acquiring next swapchain image")
    return false;
  }

  outputView = m_context->swapchain().imageView(currentFrame->frameNumber);

  return true;
}

bool FrameManager::endFrame() {
  ASSERT(m_context != nullptr)
  auto &f = m_frames[m_currentFrame];

  // Submit to queue
  auto submitInfo = createSubmitInfo(f);
  if (!m_context->device().graphicsQueue().submit(submitInfo, f.fence.handle()))
    return false;

  // Present to swapchain
  if (m_context->swapchain().presentImage(
          f.frameNumber, f.renderFinished.handle()) != VK_SUCCESS) {
    LOGE("Error presenting swapchain image")
    return false;
  }

  // Advance frame counter
  m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;

  return true;
}

VkSubmitInfo FrameManager::createSubmitInfo(const Frame &frame) const {
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {frame.imageAvailable.handle()};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  VkCommandBuffer cmdBuffers[] = {frame.cmdBuffer.handle()};
  submitInfo.pCommandBuffers = cmdBuffers;
  submitInfo.commandBufferCount = 1;

  VkSemaphore signalSemaphores[] = {frame.renderFinished.handle()};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  return submitInfo;
}

} // namespace vvhl