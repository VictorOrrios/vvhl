#include <vvhl/Resources/ResourceManager.hpp>

namespace vvhl {

bool ResourceManager::initialize(VulkanContext &context) {
  m_context = &context;
  return true;
}

void ResourceManager::destroy() {
  m_buffers.forEach([](Buffer &buffer) { buffer.destroy(); });

  m_images.forEach([](Image &image) { image.destroy(); });

  m_samplers.forEach([](Sampler &sampler) { sampler.destroy(); });

  m_buffers.clear();
  m_images.clear();
  m_samplers.clear();

  m_context = nullptr;
}

BufferHandle ResourceManager::createBuffer(const BufferDescription &desc) {
  ASSERT(m_context);
  BufferHandle handle = m_buffers.emplace();
  if (!m_buffers.get(handle).create(m_context->allocator(), desc)) {
    LOGE("Failed at creating a buffer in the resource manager")
    return {UINT32_MAX, UINT32_MAX};
  };
  return handle;
}

ImageHandle ResourceManager::createImage(const ImageDescription &desc) {
  ASSERT(m_context);
  ImageHandle handle = m_images.emplace();
  if (!m_images.get(handle).create(*m_context, desc)) {
    LOGE("Failed at creating an image in the resource manager")
    return {UINT32_MAX, UINT32_MAX};
  };
  return handle;
}

SamplerHandle ResourceManager::createSampler(const SamplerDescription &desc) {
  ASSERT(m_context);
  SamplerHandle handle = m_samplers.emplace();
  if (!m_samplers.get(handle).create(m_context->device(), desc)) {
    LOGE("Failed at creating a sampler in the resource manager")
    return {UINT32_MAX, UINT32_MAX};
  };
  return handle;
}

Buffer &ResourceManager::buffer(BufferHandle handle) {
  return m_buffers.get(handle);
}

const Buffer &ResourceManager::buffer(BufferHandle handle) const {
  return m_buffers.get(handle);
}

Image &ResourceManager::image(ImageHandle handle) {
  return m_images.get(handle);
}

const Image &ResourceManager::image(ImageHandle handle) const {
  return m_images.get(handle);
}

Sampler &ResourceManager::sampler(SamplerHandle handle) {
  return m_samplers.get(handle);
}

const Sampler &ResourceManager::sampler(SamplerHandle handle) const {
  return m_samplers.get(handle);
}

bool ResourceManager::valid(BufferHandle handle) {
  return m_buffers.valid(handle);
}

bool ResourceManager::valid(ImageHandle handle) {
  return m_images.valid(handle);
}

bool ResourceManager::valid(SamplerHandle handle) {
  return m_samplers.valid(handle);
}

void ResourceManager::destroy(BufferHandle handle) {
  m_buffers.get(handle).destroy();
  m_buffers.destroy(handle);
}

void ResourceManager::destroy(ImageHandle handle) {
  m_images.get(handle).destroy();
  m_images.destroy(handle);
}

void ResourceManager::destroy(SamplerHandle handle) {
  m_samplers.get(handle).destroy();
  m_samplers.destroy(handle);
}

} // namespace vvhl