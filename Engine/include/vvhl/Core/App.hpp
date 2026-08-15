#pragma once

#include "Vulkan/Context/VulkanContext.hpp"
#include "Vulkan/Descriptors/DescriptorPool.hpp"
#include "vvhl/Resources/ResourceManager.hpp"

namespace vvhl {

class VulkanContext;
class DescriptorPool;
class ResourceManager;

class App {
public:
  App() = default;
  ~App() { destroy(); }

  App(const App &) = delete;
  App &operator=(const App &) = delete;

  bool initialize();
  void destroy();

public:
  VulkanContext &context() { return m_context; }
  ResourceManager &resourceManager() { return m_resourceManager; }

private:
  VulkanContext m_context;
  ResourceManager m_resourceManager;
};

} // namespace vvhl