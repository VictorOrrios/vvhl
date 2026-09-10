#include <vvhl/Resources/GBuffers.hpp>

namespace vvhl {

bool GBuffers::initialize(ResourceManager &resourceManager) {
  m_resourceManager = &resourceManager;
  return true;
};

void GBuffers::destroy() { m_resourceManager = nullptr; }

void GBuffers::add(GBufferID id, ImageHandle handle) {
  ASSERT(!m_handles.contains(id))
  m_handles.insert(std::pair(id, handle));
}

ImageHandle GBuffers::get(GBufferID id){
  ASSERT(m_handles.contains(id))
  return m_handles[id];
}

void GBuffers::resize(uint32_t width, uint32_t height) {
  for (auto &it : m_handles) {
    m_resourceManager->image(it.second).recreate(width, height);
  }
}

} // namespace vvhl