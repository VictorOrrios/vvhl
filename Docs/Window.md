# Window

## Overview

`Window` is a thin wrapper around GLFW responsible for window management.

It owns the native GLFW window and translates GLFW callbacks into engine events.

---

## Responsibilities

- Create and destroy windows.
- Window state (open, close, minimize, maximize, fullscreen).
- Window size and framebuffer size.
- Vulkan surface creation.
- Register GLFW callbacks.
- Dispatch window events.

---

## Does NOT Handle

- Input state.
- Rendering.
- Vulkan initialization.
- Application logic.

---

## Notes

Wayland limitations:

- `setPosition()` may have no effect.
- `WindowMoveEvent` may never be generated.
- `WindowMinimizeEvent` may not be available.
- A Vulkan window becomes visible after the first image is presented.

These limitations come from the platform, not from the engine.
If you use other popular window manager and they still not work give me a touch (email), as it has been left untested