#pragma once

#include "Handle.hpp"
#include <vector>
#include <vvhl/Core/Assert.hpp>

namespace vvhl {

template <typename T> class SlotMap {
public:
  SlotMap() = default;

  Handle<T> emplace() {
    uint32_t index;

    if (!m_freeSlots.empty()) {
      index = m_freeSlots.back();
      m_freeSlots.pop_back();
    } else {
      index = static_cast<uint32_t>(m_slots.size());
      m_slots.emplace_back();
    }

    Slot &slot = m_slots[index];
    slot.occupied = true;

    return {index, slot.generation};
  }

  void destroy(Handle<T> handle) {
    if (m_slots.size() <= handle.index || !m_slots[handle.index].occupied)
      return;
    m_slots[handle.index].occupied = false;
    m_slots[handle.index].generation++;
    m_freeSlots.push_back(handle.index);
  }

  bool valid(Handle<T> handle) const {
    return handle.index < m_slots.size() && m_slots[handle.index].occupied &&
           m_slots[handle.index].generation == handle.generation;
  }

  T &get(Handle<T> handle) {
    ASSERT(valid(handle));
    return m_slots[handle.index].object;
  }

  const T &get(Handle<T> handle) const {
    ASSERT(valid(handle));
    return m_slots[handle.index].object;
  }

  void clear() {
    m_slots.clear();
    m_freeSlots.clear();
  }

  size_t size() const { return m_slots.size() - m_freeSlots.size(); }

  template <typename Func> void forEach(Func &&func) {
    for (Slot &slot : m_slots) {
      if (slot.occupied)
        func(slot.object);
    }
  }

private:
  struct Slot {
    T object{};

    uint32_t generation = 0;

    bool occupied = false;
  };

private:
  std::vector<Slot> m_slots;
  std::vector<uint32_t> m_freeSlots;
};
} // namespace vvhl
