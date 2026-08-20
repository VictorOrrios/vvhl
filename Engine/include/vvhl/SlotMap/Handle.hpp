#pragma once

#include <cstdint>

namespace vvhl {

template <typename T> struct Handle {
  uint32_t index = InvalidIndex;
  uint32_t generation = 0;

  bool operator==(const Handle &) const = default;

  bool valid() const { return index != InvalidIndex; }

  static constexpr uint32_t InvalidIndex = UINT32_MAX;
};

} // namespace vvhl