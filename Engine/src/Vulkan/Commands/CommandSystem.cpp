
#include "CommandSystem.hpp"
#include "Vulkan/Commands/CommandPool.hpp"

namespace vvhl {

bool CommandSystem::initialize(Device &device) {
  ASSERT(m_pools.empty())

  auto initPoolPtr = [this](CommandPool* pool, uint32_t family, Device &device) {
    pool = getOrCreatePool(device, family);
    if(pool == nullptr){
      destroy();
      return false;
    }
    return true;
  };
  
  return
  initPoolPtr(m_graphicsPool,device.graphicsFamily(),device) &&
  initPoolPtr(m_computePool,device.computeFamily(),device) &&
  initPoolPtr(m_transferPool,device.transferFamily(),device);
}

CommandPool *CommandSystem::getOrCreatePool(Device &device, uint32_t familyIndex) {
  for (auto &pool : m_pools) {
    if (pool->queueFamilyIndex() == familyIndex)
      return pool.get();
  }

  auto pool = std::make_unique<CommandPool>();

  if (!pool->initialize(device, familyIndex))
    return nullptr;

  CommandPool *result = pool.get();

  m_pools.push_back(std::move(pool));

  return result;
}

void CommandSystem::destroy() {
  for (auto &pool : m_pools)
    pool->destroy();
  m_pools.clear();
  m_graphicsPool = nullptr;
  m_computePool = nullptr;
  m_transferPool = nullptr;
}

} // namespace vvhl
