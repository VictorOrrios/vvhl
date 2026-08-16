#pragma once

#include <vvhl/Core/FrameManager.hpp>
#include <vvhl/Vulkan/Commands/CommandPool.hpp>
#include <vvhl/Vulkan/Commands/CommandSystem.hpp>
#include <vvhl/Vulkan/Context/VulkanContext.hpp>
#include <vvhl/Events/EventDispatcher.hpp>
#include <vvhl/Resources/ResourceManager.hpp>

namespace vvhl {

class VulkanContext;
class DescriptorPool;
class ResourceManager;

struct AppConfig {
  WindowSpecification windowSpec;
};

class App {
public:
  App() = default;
  ~App() { destroy(); }

  App(const App &) = delete;
  App &operator=(const App &) = delete;

  virtual void destroy();

  void run();

public:
  VulkanContext &context() { return m_context; }
  ResourceManager &resourceManager() { return m_resourceManager; }

protected:
  bool initializeBase(const AppConfig &config);
  void destroyBase();

protected:
  virtual void onRender(VkCommandBuffer cmdBuff, VkImageView outputView, uint32_t currentFrame);
  // on resize

private:
  void submitCmd();

private:
  Window m_window;
  EventDispatcher m_eventDispatcher;
  VulkanContext m_context;
  ResourceManager m_resourceManager;
  CommandSystem m_cmdSystem;
  FrameManager m_frameManager;

  CommandPool* m_cmdPool = nullptr;

  bool m_shouldClose = false;
};

} // namespace vvhl