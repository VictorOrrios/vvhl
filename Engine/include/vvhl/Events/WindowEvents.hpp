#pragma once
#include <vvhl/Events/Event.hpp>
#include <vvhl/Core/Window.hpp>

namespace vvhl
{

class WindowEvent : public Event{
public:
    explicit WindowEvent(Window* window)
        : WindowPtr(window)
    {}

    Window* WindowPtr;
};

//------------------------------------------------------------

class WindowResizeEvent final : public WindowEvent{
public:
    WindowResizeEvent(Window* window,
                      uint32_t width,
                      uint32_t height)
        : WindowEvent(window),
          Width(width),
          Height(height)
    {}

    uint32_t Width;
    uint32_t Height;
};

//------------------------------------------------------------

class FramebufferResizeEvent final : public WindowEvent{
public:
    FramebufferResizeEvent(Window* window,
                           uint32_t width,
                           uint32_t height)
        : WindowEvent(window),
          Width(width),
          Height(height)
    {}

    uint32_t Width;
    uint32_t Height;
};

//------------------------------------------------------------

class WindowCloseEvent final : public WindowEvent{
public:
    explicit WindowCloseEvent(Window* window)
        : WindowEvent(window)
    {}
};

//------------------------------------------------------------

class WindowFocusEvent final : public WindowEvent{
public:
    WindowFocusEvent(Window* window,
                     bool focused)
        : WindowEvent(window),
          Focused(focused)
    {}

    bool Focused;
};

//------------------------------------------------------------

class WindowMinimizeEvent final : public WindowEvent{
public:
    WindowMinimizeEvent(Window* window,
                        bool minimized)
        : WindowEvent(window),
          Minimized(minimized)
    {}

    bool Minimized;
};

//------------------------------------------------------------

class WindowMaximizeEvent final : public WindowEvent{
public:
    WindowMaximizeEvent(Window* window,
                        bool maximized)
        : WindowEvent(window),
          Maximized(maximized)
    {}

    bool Maximized;
};

//------------------------------------------------------------

class WindowMoveEvent final : public WindowEvent{
public:
    WindowMoveEvent(Window* window,
                    int x,
                    int y)
        : WindowEvent(window),
          X(x),
          Y(y)
    {}

    int X;
    int Y;
};

} // namespace vvhl