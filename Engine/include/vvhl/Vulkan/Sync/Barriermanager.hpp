#pragma once

#include "vvhl/Resources/ResourceManager.hpp"
#include "vvhl/Vulkan/Commands/CommandBuffer.hpp"
#include <cstdint>
#include <vvhl/Vulkan/Memory/Buffer.hpp>
#include <vvhl/Vulkan/Memory/Image.hpp>
#include <vvhl/vvhl.hpp>

namespace vvhl {

class ResourceManager;

struct MemoryBarrier {
  VkPipelineStageFlags2 srcStageMask = VK_PIPELINE_STAGE_2_NONE;
  VkAccessFlags2 srcAccessMask = 0;
  VkPipelineStageFlags2 dstStageMask = VK_PIPELINE_STAGE_2_NONE;
  VkAccessFlags2 dstAccessMask = 0;

  MemoryBarrier *stage(VkPipelineStageFlags2 src, VkPipelineStageFlags2 dst) {
    this->srcStageMask = src;
    this->dstStageMask = dst;
    return this;
  };

  MemoryBarrier *accessMask(VkPipelineStageFlags2 src,
                            VkPipelineStageFlags2 dst) {
    this->srcAccessMask = src;
    this->dstAccessMask = dst;
    return this;
  };

  VkMemoryBarrier2 toVk() {
    return {
        .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
        .pNext = NULL,
        .srcStageMask = this->srcStageMask,
        .srcAccessMask = this->srcAccessMask,
        .dstStageMask = this->dstStageMask,
        .dstAccessMask = this->dstAccessMask,
    };
  }
};

struct BufferBarrier {
  Buffer *buffer = nullptr;
  uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  VkDeviceSize offset = 0;
  VkDeviceSize size = VK_WHOLE_SIZE;
  VkPipelineStageFlags2 srcStageMask = VK_PIPELINE_STAGE_2_NONE;
  VkAccessFlags2 srcAccessMask = 0;
  VkPipelineStageFlags2 dstStageMask = VK_PIPELINE_STAGE_2_NONE;
  VkAccessFlags2 dstAccessMask = 0;

  // Constructor
  explicit BufferBarrier(Buffer *buf);

  // Chainable methods
  BufferBarrier *stage(VkPipelineStageFlags2 src, VkPipelineStageFlags2 dst);
  BufferBarrier *access(VkAccessFlags2 src, VkAccessFlags2 dst);
  BufferBarrier *toAccess(VkAccessFlags2 dst);
  BufferBarrier *queueFamily(uint32_t src, uint32_t dst);
  BufferBarrier *fromOffset(VkDeviceSize offset);
  BufferBarrier *withSize(VkDeviceSize size);

  VkBufferMemoryBarrier2 toVk() const;
};

struct ImageBarrier {
  Image *image = nullptr;
  uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  VkPipelineStageFlags2 srcStageMask = VK_PIPELINE_STAGE_2_NONE;
  VkAccessFlags2 srcAccessMask = 0;
  VkPipelineStageFlags2 dstStageMask = VK_PIPELINE_STAGE_2_NONE;
  VkAccessFlags2 dstAccessMask = 0;
  VkImageLayout oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  VkImageLayout newLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  uint32_t baseMipLevel = 0;
  uint32_t levelCount = VK_REMAINING_MIP_LEVELS;
  uint32_t baseArrayLayer = 0;
  uint32_t layerCount = VK_REMAINING_ARRAY_LAYERS;

  // Constructor
  explicit ImageBarrier(Image *img);

  // Chainable methods
  ImageBarrier *stage(VkPipelineStageFlags2 src, VkPipelineStageFlags2 dst);
  ImageBarrier *access(VkAccessFlags2 src, VkAccessFlags2 dst);
  ImageBarrier *toAccess(VkAccessFlags2 dst);
  ImageBarrier *layout(VkImageLayout src, VkImageLayout dst);
  ImageBarrier *toLayout(VkImageLayout dst);
  ImageBarrier *mip(uint32_t baseMipLevel, uint32_t mipLevelCount);
  ImageBarrier *array(uint32_t baseArrayLayer, uint32_t layerCount);
  ImageBarrier *queueFamily(uint32_t src, uint32_t dst);

  VkImageMemoryBarrier2 toVk() const;
};

class BarrierManager {

public:
  BarrierManager() = default;
  ~BarrierManager() { destroy(); };

  BarrierManager(const BarrierManager &) = delete;
  BarrierManager &operator=(const BarrierManager &) = delete;

  bool initialize(ResourceManager &resourceManager);
  void destroy();

  MemoryBarrier *memoryBarrier();
  BufferBarrier *bufferBarrier(BufferHandle buffer);
  ImageBarrier *imageBarrier(ImageHandle image);
  BufferBarrier *bufferBarrier(Buffer &buffer);
  ImageBarrier *imageBarrier(Image &image);

  void submit(CommandBuffer cmd, VkDependencyFlags flags = 0);

private:
  ResourceManager *m_resourceManager = nullptr;
  std::vector<MemoryBarrier> m_memoryBarriers;
  std::vector<BufferBarrier> m_bufferBarriers;
  std::vector<ImageBarrier> m_imageBarriers;
};

} // namespace vvhl