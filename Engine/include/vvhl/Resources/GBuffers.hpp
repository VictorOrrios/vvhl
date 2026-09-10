#pragma once

#include <vvhl/Resources/ResourceManager.hpp>

namespace vvhl {

class ResourceManager;

using GBufferID = uint32_t;

class GBuffers {
public:
  GBuffers() = default;
  ~GBuffers() = default;

  GBuffers(const GBuffers &) = delete;
  GBuffers &operator=(const GBuffers &) = delete;

  bool initialize(ResourceManager &resourceManager);
  void destroy();

  void add(GBufferID id, ImageHandle handle);
  ImageHandle get(GBufferID id);
  void resize(uint32_t width, uint32_t height);

private:
  ResourceManager *m_resourceManager = nullptr;
  std::unordered_map<GBufferID, ImageHandle> m_handles;
};

} // namespace vvhl