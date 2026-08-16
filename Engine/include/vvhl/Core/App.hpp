#pragma once

#include <vvhl/Core/Window.hpp>
#include <vvhl/Vulkan/Descriptors/DescriptorSet.hpp>
#include <vvhl/Core/FrameManager.hpp>
#include <vvhl/Events/EventDispatcher.hpp>
#include <vvhl/ImGui/ImGuiLayer.hpp>
#include <vvhl/Resources/ResourceManager.hpp>
#include <vvhl/Vulkan/Commands/CommandPool.hpp>
#include <vvhl/Vulkan/Commands/CommandSystem.hpp>
#include <vvhl/Vulkan/Context/VulkanContext.hpp>

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
  ~App() = default;

  App(const App &) = delete;
  App &operator=(const App &) = delete;

  virtual void destroy() { destroyBase(); };

  void run();

public:
  VulkanContext &context() { return m_context; }
  ResourceManager &resourceManager() { return m_resourceManager; }
  virtual void onRender(VkCommandBuffer, uint32_t) {};

protected:
  bool initializeBase(const AppConfig &config);
  void destroyBase();

protected:
  // on resize

protected:
  ImageHandle m_viewport;
  SamplerHandle m_viewportSampler;
  VkDescriptorSet m_viewportSet;

private:
  void renderGUI();
  bool createViewport();

private:
  Window m_window;
  EventDispatcher m_eventDispatcher;
  VulkanContext m_context;
  ResourceManager m_resourceManager;
  CommandSystem m_cmdSystem;
  FrameManager m_frameManager;
  ImGuiLayer m_imguiLayer;
  DescriptorSet m_descSet;
  CommandPool *m_cmdPool = nullptr;

  bool m_shouldClose = false;
};

} // namespace vvhl