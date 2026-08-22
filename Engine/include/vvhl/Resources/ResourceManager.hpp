#pragma once

#include <vvhl/Vulkan/Context/VulkanContext.hpp>
#include <vvhl/Vulkan/Memory/Buffer.hpp>
#include <vvhl/Vulkan/Memory/Image.hpp>
#include <vvhl/Vulkan/Memory/Sampler.hpp>

namespace vvhl {
  
struct BufferCreateDescription;
struct ImageCreateDescription;
struct SamplerDescription;
class Buffer;
class Image;
class Sampler;
class VulkanContext;
using BufferHandle = Handle<Buffer>;
using ImageHandle = Handle<Image>;
using SamplerHandle = Handle<Sampler>;

class ResourceManager {
public:
  ResourceManager() = default;
  ~ResourceManager() = default;

  ResourceManager(const ResourceManager &) = delete;
  ResourceManager &operator=(const ResourceManager &) = delete;

  bool initialize(VulkanContext &context);
  void destroy();

  BufferHandle createBuffer(const BufferCreateDescription &desc);

  ImageHandle createImage(const ImageCreateDescription &desc);

  SamplerHandle createSampler(const SamplerDescription &desc);

  Buffer &buffer(BufferHandle handle);
  const Buffer &buffer(BufferHandle handle) const;

  Image &image(ImageHandle handle);
  const Image &image(ImageHandle handle) const;

  Sampler &sampler(SamplerHandle handle);
  const Sampler &sampler(SamplerHandle handle) const;

  bool valid(BufferHandle handle);

  bool valid(ImageHandle handle);

  bool valid(SamplerHandle handle);

  void destroy(BufferHandle handle);

  void destroy(ImageHandle handle);

  void destroy(SamplerHandle handle);

private:
  VulkanContext *m_context;
  SlotMap<Buffer> m_buffers;
  SlotMap<Image> m_images;
  SlotMap<Sampler> m_samplers;
};

} // namespace vvhl