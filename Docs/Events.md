# Event Dispatcher

## Overview

The Event Dispatcher provides a lightweight publish/subscribe system used throughout the engine.

Events are emitted by engine systems (Window, Input, Renderer, etc.) and delivered to every subscribed listener.

The dispatcher is owned by the `Application` and shared with all engine modules.

---

## Event Flow

```
Engine System -> EventDispatcher -> Subscribers
```

Example:

```
GLFW -> WindowResizeEvent -> EventDispatcher ->
Application,Renderer,UI,Editor,...
```

---

## Usage

Subscribe to an event:

```cpp
dispatcher.subscribe<MyEvent>(
[](const MyEvent& e)
{
    int foo = e.bar*2;
    ...
});
```

Dispatch an event:

```cpp
dispatcher.dispatch(MyEvent(...));
```

---

## Design

- Single dispatcher per application.
- No event filtering.
- No event priorities.
- No event consumption.
- Every subscriber receives every event of the subscribed type.
