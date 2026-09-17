#pragma once

class Event {
public:
  virtual ~Event() = default;
  // Only enqueue the latest event and discard others if true
  static constexpr bool Coalescable = false;
};