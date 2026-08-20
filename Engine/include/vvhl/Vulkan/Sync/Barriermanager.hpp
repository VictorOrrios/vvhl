#pragma once

#include "vvhl/Resources/ResourceManager.hpp"
#include "vvhl/Vulkan/Commands/CommandBuffer.hpp"
#include <cstdint>
#include <vulkan/vulkan_core.h>
#include <vvhl/vvhl.hpp>

namespace vvhl {

class BarrierManager {

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
    Buffer *buffer;
    VkPipelineStageFlags2 srcStageMask = VK_PIPELINE_STAGE_2_NONE;
    VkAccessFlags2 srcAccessMask = 0;
    VkPipelineStageFlags2 dstStageMask = VK_PIPELINE_STAGE_2_NONE;
    VkAccessFlags2 dstAccessMask = 0;
    uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    VkDeviceSize offset = 0;
    VkDeviceSize size = VK_WHOLE_SIZE;

    BufferBarrier *stage(VkPipelineStageFlags2 src, VkPipelineStageFlags2 dst) {
      this->srcStageMask = src;
      this->dstStageMask = dst;
      return this;
    };

    BufferBarrier *accessMask(VkPipelineStageFlags2 src,
                              VkPipelineStageFlags2 dst) {
      this->srcAccessMask = src;
      this->dstAccessMask = dst;
      return this;
    };

    BufferBarrier *toAccessMask(VkPipelineStageFlags2 dst) {
      this->srcAccessMask = buffer->syncState().access;
      this->dstAccessMask = dst;
      return this;
    };

    BufferBarrier *queueFamilyIndex(uint32_t src, uint32_t dst) {
      this->srcQueueFamilyIndex = src;
      this->dstQueueFamilyIndex = dst;
      return this;
    }

    BufferBarrier *fromOffset(VkDeviceSize offset = 0) {
      this->offset = offset;
      return this;
    }

    BufferBarrier *toSize(VkDeviceSize size = VK_WHOLE_SIZE) {
      this->size = size;
      return this;
    }

    VkBufferMemoryBarrier2 toVk() {
      return {.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
              .pNext = NULL,
              .srcStageMask = this->srcStageMask,
              .srcAccessMask = this->srcAccessMask,
              .dstStageMask = this->dstStageMask,
              .dstAccessMask = this->dstAccessMask,
              .srcQueueFamilyIndex = this->srcQueueFamilyIndex,
              .dstQueueFamilyIndex = this->dstQueueFamilyIndex,
              .buffer = buffer->handle(),
              .offset = offset,
              .size = size};
    }
  };

  struct ImageBarrier {
    Image *image;
    uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    VkPipelineStageFlags2 srcStageMask = VK_PIPELINE_STAGE_2_NONE;
    VkAccessFlags2 srcAccessMask = 0;
    VkPipelineStageFlags2 dstStageMask = VK_PIPELINE_STAGE_2_NONE;
    VkAccessFlags2 dstAccessMask = 0;
    // Esto funciona bien? Si se crea un ImageBarrier con solo {&miImagen} se
    // rellena todo el resto bien?
    VkImageLayout oldLayout = image->layout();
    VkImageLayout newLayout = image->layout();
    uint32_t baseMipLevel = image->syncState().baseMipLevel;
    uint32_t levelCount = image->syncState().mipLevelCount;
    uint32_t baseArrayLayer = image->syncState().baseArrayLayer;
    uint32_t layerCount = image->syncState().arrayLayerCount;

    ImageBarrier *stage(VkPipelineStageFlags2 src, VkPipelineStageFlags2 dst) {
      this->srcStageMask = src;
      this->dstStageMask = dst;
      return this;
    };

    ImageBarrier *accessMask(VkPipelineStageFlags2 src,
                             VkPipelineStageFlags2 dst) {
      this->srcAccessMask = src;
      this->dstAccessMask = dst;
      return this;
    };

    ImageBarrier *toAccessMask(VkPipelineStageFlags2 dst) {
      this->srcAccessMask = image->syncState().access;
      this->dstAccessMask = dst;
      return this;
    }

    ImageBarrier *layout(VkImageLayout src, VkImageLayout dst) {
      this->oldLayout = src;
      this->newLayout = dst;
      return this;
    }

    ImageBarrier *toLayout(VkImageLayout dst) {
      this->oldLayout = image->layout();
      this->newLayout = dst;
      return this;
    }

    ImageBarrier *mip(uint32_t baseMipLevel, uint32_t mipLevelCount) {
      this->baseMipLevel = baseMipLevel;
      this->levelCount = mipLevelCount;
      return this;
    }

    ImageBarrier *array(uint32_t baseArrayLayer, uint32_t layerCount) {
      this->baseArrayLayer = baseArrayLayer;
      this->layerCount = layerCount;
      return this;
    }

    VkImageMemoryBarrier2 toVk() {
      return {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
              .pNext = NULL,
              .srcStageMask = this->srcStageMask,
              .srcAccessMask = this->srcAccessMask,
              .dstStageMask = this->dstStageMask,
              .dstAccessMask = this->dstAccessMask,
              .oldLayout = this->oldLayout,
              .newLayout = this->newLayout,
              .srcQueueFamilyIndex = this->srcQueueFamilyIndex,
              .dstQueueFamilyIndex = this->dstQueueFamilyIndex,
              .image = image->handle(),
              .subresourceRange = {
                  .aspectMask = image->description().aspectMask,
                  .baseMipLevel = this->baseMipLevel,
                  .levelCount = this->levelCount,
                  .baseArrayLayer = this->baseArrayLayer,
                  .layerCount = this->layerCount,
              }};
    }
  };

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

  void submit(CommandBuffer cmd, VkDependencyFlags flags = 0);

private:
  ResourceManager *m_resourceManager = nullptr;
  std::vector<MemoryBarrier> m_memoryBarriers;
  std::vector<BufferBarrier> m_bufferBarriers;
  std::vector<ImageBarrier> m_imageBarriers;
};

} // namespace vvhl