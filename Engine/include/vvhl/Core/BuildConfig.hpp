#pragma once

namespace BuildConfig {

#ifdef BUILD_DEBUG

constexpr bool EnableValidationLayers = true;
constexpr bool EnableAssertions = true;
constexpr bool EnableShaderHotReload = true;
constexpr bool EnableGPUMarkers = true;

#elif defined(BUILD_DEV)

constexpr bool EnableValidationLayers = false;
constexpr bool EnableAssertions = false;
constexpr bool EnableShaderHotReload = true;
constexpr bool EnableGPUMarkers = true;

#elif defined(BUILD_RELEASE)

constexpr bool EnableValidationLayers = false;
constexpr bool EnableAssertions = false;
constexpr bool EnableShaderHotReload = false;
constexpr bool EnableGPUMarkers = false;

#else

#error "Unknown build configuration."

#endif

} // namespace BuildConfig