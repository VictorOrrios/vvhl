#pragma once

#include <vvhl/Events/Event.hpp>

namespace vvhl {

class MouseEvent : public Event {
public:
  MouseEvent(double x, double y) : X(x), Y(y) {}

  double X;
  double Y;
};

//------------------------------------------------------------

class MouseMovedEvent final : public MouseEvent {
public:
  MouseMovedEvent(double x, double y) : MouseEvent(x, y) {}
};

//------------------------------------------------------------

class MouseEnteredEvent final : public Event {
public:
  explicit MouseEnteredEvent(bool entered) : Entered(entered) {}

  bool Entered;
};

//------------------------------------------------------------

class MouseScrolledEvent final : public Event {
public:
  MouseScrolledEvent(double xOffset, double yOffset)
      : XOffset(xOffset), YOffset(yOffset) {}

  double XOffset;
  double YOffset;
};

//------------------------------------------------------------

class MouseButtonEvent : public MouseEvent {
public:
  MouseButtonEvent(int button, double x, double y)
      : MouseEvent(x, y), Button(button) {}

  int Button;
};

//------------------------------------------------------------

class MouseButtonPressedEvent final : public MouseButtonEvent {
public:
  MouseButtonPressedEvent(int button, double x, double y)
      : MouseButtonEvent(button, x, y) {}
};

//------------------------------------------------------------

class MouseButtonReleasedEvent final : public MouseButtonEvent {
public:
  MouseButtonReleasedEvent(int button, double x, double y)
      : MouseButtonEvent(button, x, y) {}
};

} // namespace vvhl