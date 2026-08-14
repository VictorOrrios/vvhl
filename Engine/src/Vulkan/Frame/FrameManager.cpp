#include "./FrameManager.hpp"

namespace vvhl {

bool FrameManager::initialize(Device &device, CommandPool &cmdPool) {

  bool success = true;

  m_frames.reserve(m_maxFramesInFlight);
  for (uint32_t i = 0; i < m_maxFramesInFlight; i++) {
    Frame f = {.frameNumber = i,
               .fence = Fence(),
               .imageAvailable = Semaphore(),
               .renderFinished = Semaphore(),
               .cmdBuffer = m_cmdPool->allocate()};

    success &= f.fence.initialize(device);
    success &= f.imageAvailable.initialize(device);
    success &= f.renderFinished.initialize(device);

    m_frames.push_back(std::move(f));
  }

  m_device = &device;
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
  m_device = nullptr;
  m_cmdPool = nullptr;
  m_currentFrame = 0;
}

bool FrameManager::beginFrame() {
  ASSERT(m_device != nullptr)

  auto &currentFrame = m_frames[m_currentFrame];

  if (currentFrame.fence.wait() != VK_SUCCESS) {
    LOGE("Error waiting at main frame fence")
    return false;
  }

  if (currentFrame.fence.reset() != VK_SUCCESS) {
    LOGE("Error reseting main frame fence")
    return false;
  }

  if (!currentFrame.cmdBuffer.reset())
    return false;

  return true;
}

void FrameManager::endFrame() {
  ASSERT(m_device != nullptr)
  m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;
}
} // namespace vvhl