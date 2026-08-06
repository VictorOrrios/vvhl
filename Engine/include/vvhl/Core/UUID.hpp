#pragma once

namespace vvhl {

class UUID {
public:
  UUID();
  explicit UUID(uint64_t uuid);

  constexpr uint64_t value() const noexcept { return m_uuid; }

  constexpr operator uint64_t() const noexcept { return m_uuid; }

  bool operator==(const UUID &) const = default;

private:
  uint64_t m_uuid = 0;
};

} // namespace vvhl

template <> struct std::hash<vvhl::UUID> {
  size_t operator()(const vvhl::UUID &uuid) const {
    return std::hash<uint64_t>()((uint64_t)uuid);
  }
};