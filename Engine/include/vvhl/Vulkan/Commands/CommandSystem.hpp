#pragma once

#include <vvhl/Vulkan/Commands/CommandPool.hpp>
#include <vvhl/Vulkan/Context/Device.hpp>

namespace vvhl {
class CommandSystem {
public:
  CommandSystem() = default;
  ~CommandSystem() { destroy(); };

  CommandSystem(const CommandSystem &) = delete;
  CommandSystem &operator=(const CommandSystem &) = delete;

  bool initialize(Device &device);
  void destroy();

public:
  CommandPool &graphicsPool() { return *m_graphicsPool; }
  CommandPool &computePool() { return *m_computePool; }
  CommandPool &transferPool() { return *m_transferPool; }

private:
  CommandPool *getOrCreatePool(Device &device, uint32_t familyIndex);

private:
  std::vector<std::unique_ptr<CommandPool>> m_pools;

  CommandPool *m_graphicsPool = nullptr;
  CommandPool *m_computePool = nullptr;
  CommandPool *m_transferPool = nullptr;
};

} // namespace vvhl