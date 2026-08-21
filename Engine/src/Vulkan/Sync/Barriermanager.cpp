#include "vvhl/Vulkan/Memory/Buffer.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>
#include <vvhl/Vulkan/Sync/Barriermanager.hpp>

namespace vvhl {
//=======================
// Buffer barrier
//=======================

BufferBarrier::BufferBarrier(Buffer *buf) : buffer(buf) {
  if (buffer) {
    srcAccessMask = buffer->syncState().access;
  }
}

BufferBarrier *BufferBarrier::stage(VkPipelineStageFlags2 src,
                                    VkPipelineStageFlags2 dst) {
  this->srcStageMask = src;
  this->dstStageMask = dst;
  return this;
}

BufferBarrier *BufferBarrier::access(VkAccessFlags2 src, VkAccessFlags2 dst) {
  this->srcAccessMask = src;
  this->dstAccessMask = dst;
  return this;
}

BufferBarrier *BufferBarrier::toAccess(VkAccessFlags2 dst) {
  if (buffer) {
    this->srcAccessMask = buffer->syncState().access;
  }
  this->dstAccessMask = dst;
  return this;
}

BufferBarrier *BufferBarrier::queueFamily(uint32_t src, uint32_t dst) {
  this->srcQueueFamilyIndex = src;
  this->dstQueueFamilyIndex = dst;
  return this;
}

BufferBarrier *BufferBarrier::fromOffset(VkDeviceSize offset) {
  this->offset = offset;
  return this;
}

BufferBarrier *BufferBarrier::withSize(VkDeviceSize size) {
  this->size = size;
  return this;
}

VkBufferMemoryBarrier2 BufferBarrier::toVk() const {
  return {
      .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
      .pNext = nullptr,
      .srcStageMask = this->srcStageMask,
      .srcAccessMask = this->srcAccessMask,
      .dstStageMask = this->dstStageMask,
      .dstAccessMask = this->dstAccessMask,
      .srcQueueFamilyIndex = this->srcQueueFamilyIndex,
      .dstQueueFamilyIndex = this->dstQueueFamilyIndex,
      .buffer = buffer ? buffer->handle() : VK_NULL_HANDLE,
      .offset = this->offset,
      .size = this->size,
  };
}

//=======================
// Image barrier
//=======================

ImageBarrier::ImageBarrier(Image *img) : image(img) {
  if (image) {
    oldLayout = image->layout();
    newLayout = image->layout();
    srcAccessMask = image->syncState().access;
    baseMipLevel = image->syncState().baseMipLevel;
    levelCount = image->syncState().mipLevelCount;
    baseArrayLayer = image->syncState().baseArrayLayer;
    layerCount = image->syncState().arrayLayerCount;
  }
}

ImageBarrier *ImageBarrier::stage(VkPipelineStageFlags2 src,
                                  VkPipelineStageFlags2 dst) {
  this->srcStageMask = src;
  this->dstStageMask = dst;
  return this;
}

ImageBarrier *ImageBarrier::access(VkAccessFlags2 src, VkAccessFlags2 dst) {
  this->srcAccessMask = src;
  this->dstAccessMask = dst;
  return this;
}

ImageBarrier *ImageBarrier::toAccess(VkAccessFlags2 dst) {
  if (image) {
    this->srcAccessMask = image->syncState().access;
  }
  this->dstAccessMask = dst;
  return this;
}

ImageBarrier *ImageBarrier::layout(VkImageLayout src, VkImageLayout dst) {
  this->oldLayout = src;
  this->newLayout = dst;
  return this;
}

ImageBarrier *ImageBarrier::toLayout(VkImageLayout dst) {
  if (image) {
    this->oldLayout = image->layout();
  }
  this->newLayout = dst;
  return this;
}

ImageBarrier *ImageBarrier::mip(uint32_t baseMipLevel, uint32_t mipLevelCount) {
  this->baseMipLevel = baseMipLevel;
  this->levelCount = mipLevelCount;
  return this;
}

ImageBarrier *ImageBarrier::array(uint32_t baseArrayLayer,
                                  uint32_t layerCount) {
  this->baseArrayLayer = baseArrayLayer;
  this->layerCount = layerCount;
  return this;
}

ImageBarrier *ImageBarrier::queueFamily(uint32_t src, uint32_t dst) {
  this->srcQueueFamilyIndex = src;
  this->dstQueueFamilyIndex = dst;
  return this;
}

VkImageMemoryBarrier2 ImageBarrier::toVk() const {
  return {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
      .pNext = nullptr,
      .srcStageMask = this->srcStageMask,
      .srcAccessMask = this->srcAccessMask,
      .dstStageMask = this->dstStageMask,
      .dstAccessMask = this->dstAccessMask,
      .oldLayout = this->oldLayout,
      .newLayout = this->newLayout,
      .srcQueueFamilyIndex = this->srcQueueFamilyIndex,
      .dstQueueFamilyIndex = this->dstQueueFamilyIndex,
      .image = image ? image->handle() : VK_NULL_HANDLE,
      .subresourceRange =
          {
              .aspectMask = image ? image->description().aspectMask
                                  : static_cast<VkImageAspectFlags>(
                                        VK_IMAGE_ASPECT_COLOR_BIT),
              .baseMipLevel = this->baseMipLevel,
              .levelCount = this->levelCount,
              .baseArrayLayer = this->baseArrayLayer,
              .layerCount = this->layerCount,
          },
  };
}

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

MemoryBarrier *BarrierManager::memoryBarrier() {
  m_memoryBarriers.emplace_back();
  return &m_memoryBarriers.back();
}

BufferBarrier *BarrierManager::bufferBarrier(BufferHandle buffer) {
  ASSERT(m_resourceManager != nullptr)
  m_bufferBarriers.emplace_back(&m_resourceManager->buffer(buffer));
  return &m_bufferBarriers.back();
}

ImageBarrier *BarrierManager::imageBarrier(ImageHandle image) {
  ASSERT(m_resourceManager != nullptr)
  m_imageBarriers.emplace_back(&m_resourceManager->image(image));
  return &m_imageBarriers.back();
}

BufferBarrier *BarrierManager::bufferBarrier(Buffer &buffer) {
  m_bufferBarriers.emplace_back(&buffer);
  return &m_bufferBarriers.back();
}

ImageBarrier *BarrierManager::imageBarrier(Image &image) {
  m_imageBarriers.emplace_back(&image);
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