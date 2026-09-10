#pragma once
#include <cstdint>
#include <vvhl/Events/Event.hpp>

namespace vvhl {

class ViewportResizeEvent : public Event {
public:
  explicit ViewportResizeEvent(uint32_t width, uint32_t height)
      : width(width), height(height) {}

  uint32_t width;
  uint32_t height;
};

} // namespace vvhl