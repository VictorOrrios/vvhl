#pragma once

#include <vvhl/Vulkan/Commands/CommandPool.hpp>
#include <vvhl/Vulkan/Context/VulkanContext.hpp>
#include <vvhl/Vulkan/Sync/Fence.hpp>
#include <vvhl/Vulkan/Sync/Semaphore.hpp>
#include <vvhl/Core/EngineConfig.hpp>

namespace vvhl {

class FrameManager {
public:
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

  bool initialize(VulkanContext &context, CommandPool &cmdPool);
  void destroy();

  bool beginFrame(Frame*& currentFrame, VkImageView& outputView);
  bool endFrame();

  VkSubmitInfo createSubmitInfo(const Frame &frame) const;

public:
  uint32_t getCurrentFrameIndex() const { return m_currentFrame; }
  Frame *getCurrentFrame() { return &m_frames[m_currentFrame]; }

private:
  VulkanContext *m_context = nullptr;
  CommandPool *m_cmdPool = nullptr;
  uint32_t m_currentFrame = 0;
  std::vector<Frame> m_frames;

  const uint32_t m_maxFramesInFlight = EngineSettings::maxFramesInFlight();
};

} // namespace vvhl