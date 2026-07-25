# Rendering Framework Architecture

## Goal

Build a lightweight rendering framework for graphics research, not a full game engine. The engine should make implementing new rendering techniques as simple as adding a new render pass.

## Core Components

### ResourceManager

Owns all GPU resources and their lifetime.

- Buffers
- Textures
- Meshes
- Pipelines
- Materials
- Shaders

No other system should create Vulkan objects directly.

---

### ResourceRegistry

A registry containing the resources available during the current frame.

Example resources:

- Depth
- Normals
- HDR
- ShadowMap
- SSAO
- Bloom

Render passes request resources from the registry instead of storing references to each other.

---

### FrameContext

Contains everything required to render a frame.

```cpp
struct FrameContext
{
    CommandBuffer* cmd;
    Scene* scene;
    Camera* camera;
    ResourceRegistry* resources;

    uint32_t frameIndex;
    float deltaTime;
};
```

It exposes the rendering context without exposing Vulkan internals.

---

### RenderPass

Each rendering technique is an independent class.

```cpp
class RenderPass
{
public:
    virtual void Execute(FrameContext& ctx) = 0;
};
```

A pass only knows:

- Which resources it reads
- Which resources it writes

---

### RenderGraph

Executes render passes in order.

Example:

```
Geometry → Shadows → SSAO → Bloom → Tonemap → Present
```

In the future it can manage dependencies, barriers, and resource creation automatically.

---

## Design Principle

A render pass should only answer two questions:

- What resources do I need?
- What resources do I produce?

Everything else should be handled by the framework.