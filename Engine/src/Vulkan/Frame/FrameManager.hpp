#pragma once

#include "Vulkan/Commands/CommandBuffer.hpp"
#include "Vulkan/Commands/CommandPool.hpp"
#include "Vulkan/Sync/Fence.hpp"
#include "Vulkan/Sync/Semaphore.hpp"
#include "vvhl/Core/EngineConfig.hpp"

namespace vvhl {

class FrameManager {

  struct Frame {
    uint32_t frameNumber;
    Fence fence;
    Semaphore imageAvailable;
    Semaphore renderFinished;
    CommandBuffer cmdBuffer;
  };

public:
  FrameManager() = default;
  ~FrameManager() { destroy(); };

  FrameManager(const FrameManager &) = delete;
  FrameManager &operator=(const FrameManager &) = delete;

  bool initialize(Device& device, CommandPool& cmdPool);
  void destroy();

  bool beginFrame();
  void endFrame();

public:
  uint32_t getCurrentFrameIndex() const { return m_currentFrame; }
  Frame *getCurrentFrame() { return &m_frames[m_currentFrame]; }

private:
  Device *m_device = nullptr;
  CommandPool *m_cmdPool = nullptr;
  uint32_t m_currentFrame = 0;
  std::vector<Frame> m_frames;

  const uint32_t m_maxFramesInFlight = EngineSettings::maxFramesInFlight();
};

} // namespace vvhl