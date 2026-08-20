#include "vvhl/Vulkan/Memory/Buffer.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>
#include <vvhl/Vulkan/Sync/Barriermanager.hpp>

namespace vvhl {

bool BarrierManager::initialize(ResourceManager &resourceManager) {
  m_resourceManager = &resourceManager;
  return true;
}

void BarrierManager::destroy() {
  m_memoryBarriers.clear();
  m_bufferBarriers.clear();
  m_imageBarriers.clear();
  m_resourceManager = nullptr;
}

BarrierManager::MemoryBarrier *BarrierManager::memoryBarrier() {
  m_memoryBarriers.push_back({});
  return &m_memoryBarriers.back();
}

BarrierManager::BufferBarrier *
BarrierManager::bufferBarrier(BufferHandle buffer) {
  m_bufferBarriers.push_back({.buffer = &m_resourceManager->buffer(buffer)});
  return &m_bufferBarriers.back();
}

BarrierManager::ImageBarrier *BarrierManager::imageBarrier(ImageHandle image) {
  m_imageBarriers.push_back({.image = &m_resourceManager->image(image)});
  return &m_imageBarriers.back();
}

void BarrierManager::submit(CommandBuffer cmd, VkDependencyFlags flags) {

  std::vector<VkMemoryBarrier2> memBarriers;
  memBarriers.reserve(m_memoryBarriers.size());
  for (auto &barrier : m_memoryBarriers) {
    memBarriers.push_back(barrier.toVk());
  }

  std::vector<VkBufferMemoryBarrier2> bufBarriers;
  bufBarriers.reserve(m_bufferBarriers.size());
  for (auto &barrier : m_bufferBarriers) {
    BufferSyncState newState;
    newState.access = barrier.dstAccessMask;

    barrier.buffer->setSyncState(newState);

    bufBarriers.push_back(barrier.toVk());
  }

  std::vector<VkImageMemoryBarrier2> imgBarriers;
  imgBarriers.reserve(m_imageBarriers.size());
  for (auto &barrier : m_imageBarriers) {
    ImageSyncState newState;
    newState.access = barrier.dstAccessMask;
    newState.layout = barrier.newLayout;
    newState.baseMipLevel = barrier.baseMipLevel;
    newState.mipLevelCount = barrier.levelCount;
    newState.baseArrayLayer = barrier.baseArrayLayer;
    newState.arrayLayerCount = barrier.layerCount;

    barrier.image->setSyncState(newState);

    barrier.image->setSyncState(newState);
    imgBarriers.push_back(barrier.toVk());
  }

  VkDependencyInfo depInfo;
  depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
  depInfo.dependencyFlags = flags;
  depInfo.memoryBarrierCount = static_cast<uint32_t>(memBarriers.size());
  depInfo.pMemoryBarriers = memBarriers.empty() ? nullptr : memBarriers.data();
  depInfo.bufferMemoryBarrierCount = static_cast<uint32_t>(bufBarriers.size());
  depInfo.pBufferMemoryBarriers =
      bufBarriers.empty() ? nullptr : bufBarriers.data();
  depInfo.imageMemoryBarrierCount = static_cast<uint32_t>(imgBarriers.size());
  depInfo.pImageMemoryBarriers =
      imgBarriers.empty() ? nullptr : imgBarriers.data();

  vkCmdPipelineBarrier2(cmd.handle(), &depInfo);

  m_memoryBarriers.clear();
  m_bufferBarriers.clear();
  m_imageBarriers.clear();
}

} // namespace vvhl