# Build Configurations

The engine defines three build configurations through CMake:

```cmake
target_compile_definitions(${target}
    PRIVATE
        $<$<CONFIG:Debug>:BUILD_DEBUG>
        $<$<CONFIG:Release>:BUILD_RELEASE>
        $<$<CONFIG:RelWithDebInfo>:BUILD_DEV>
)
```

These macros enable or disable engine features depending on the build type.

| Feature | `BUILD_DEBUG` | `BUILD_DEV` (`RelWithDebInfo`) | `BUILD_RELEASE` |
|----------|:-------------:|:------------------------------:|:---------------:|
| Compiler Optimizations | ❌ | ✅ | ✅ |
| Debug Symbols | ✅ | ✅ | ❌ |
| Assertions | ✅ | ❌ | ❌ |
| Vulkan Validation Layers | ✅ | ❌ | ❌ |
| Verbose Logging | ✅ | Optional | ❌ |
| GPU Profiling (Tracy) | ✅ | ✅ | ❌ |
| GPU Markers | ✅ | ✅ | ❌ |
| Shader Hot Reload | ✅ | ✅ | ❌ |
| RenderDoc Integration | ✅ | ✅ | ❌ |

## Purpose

### `BUILD_DEBUG`

Used for day-to-day development.

- Maximum debugging information.
- Validation layers enabled.
- Assertions enabled.
- Best for finding bugs.

---

### `BUILD_DEV` (`RelWithDebInfo`)

Used for graphics development and profiling.

- Compiler optimizations enabled.
- Debug symbols preserved.
- Suitable for performance analysis.
- Closest environment to Release while remaining debuggable.

---

### `BUILD_RELEASE`

Used for final performance measurements.

- Maximum compiler optimizations.
- Debug features disabled.
- Minimal runtime overhead.
- Intended to represent the final application.