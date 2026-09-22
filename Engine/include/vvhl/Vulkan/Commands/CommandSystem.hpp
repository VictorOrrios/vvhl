#pragma once

#include <vvhl/Vulkan/Commands/CommandPool.hpp>
#include <vvhl/Vulkan/Context/Device.hpp>

namespace vvhl {

class Queue;

class CommandSystem {
public:
  CommandSystem() = default;
  ~CommandSystem()  = default;

  CommandSystem(const CommandSystem &) = delete;
  CommandSystem &operator=(const CommandSystem &) = delete;

  bool initialize(Device &device);
  void destroy();

public:
  CommandPool &graphicsPool() { return *m_graphicsPool; }
  CommandPool &computePool() { return *m_computePool; }
  CommandPool &transferPool() { return *m_transferPool; }
  Queue &graphicsQueue() { return m_device->graphicsQueue();}
  Queue &computeQueue() { return m_device->computeQueue();}
  Queue &transferQueue() { return m_device->transferQueue();}

private:
  CommandPool *getOrCreatePool(Device &device, uint32_t familyIndex);

private:
  Device* m_device = nullptr;

  std::vector<std::unique_ptr<CommandPool>> m_pools;

  CommandPool *m_graphicsPool = nullptr;
  CommandPool *m_computePool = nullptr;
  CommandPool *m_transferPool = nullptr;
};

} // namespace vvhl
