#pragma once

#include "vvhl/Core/FrameManager.hpp"
#include "vvhl/Events/EventDispatcher.hpp"
#include "vvhl/Resources/GBuffers.hpp"
#include "vvhl/Resources/ResourceManager.hpp"
#include "vvhl/Vulkan/Commands/CommandSystem.hpp"
#include "vvhl/Vulkan/Context/VulkanContext.hpp"

namespace vvhl {

struct EngineContext{
  EventDispatcher* eventDispatcher = nullptr;
  VulkanContext* vkContext = nullptr;
  ResourceManager* resourceManager = nullptr;
  GBuffers* gbuffers = nullptr;
  CommandSystem* cmdSystem = nullptr;
  FrameManager* frameManager = nullptr;
};

} // namespace vvhl
