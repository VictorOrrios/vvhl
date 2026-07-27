# Input

## Overview

`Input` manages keyboard, mouse and cursor state.

It receives GLFW callbacks, updates internal state and dispatches input events.

---

## Responsibilities

- Keyboard state.
- Mouse button state.
- Mouse position.
- Mouse delta.
- Cursor visibility.
- Cursor lock.
- Input events.

---

## Usage

Query input:

```cpp
Input::isKeyPressed(KeyCode::Space);
Input::isMouseButtonPressed(MouseButton::Left);
```

Control cursor:

```cpp
Input::showCursor(window);
Input::hideCursor(window);

Input::lockCursor(window);
Input::unlockCursor(window);
```

Subscribe to input events:

```cpp
dispatcher.subscribe<KeyPressedEvent>([&](const KeyPressedEvent &e) {
    LOGI("Key {} pressed", e.Key);
    ...
});
```

---

## Frame Update

Call once every frame:

```cpp
Input::endFrame();
```

This updates the mouse delta for the next frame.

---

## Notes

Wayland limitations:

- `setCursorPosition()` is not supported.
- Relative mouse movement works normally while the cursor is locked.