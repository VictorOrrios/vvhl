#pragma once

namespace BuildConfig {

#ifdef BUILD_DEBUG

constexpr bool EnableValidationLayers = true;
constexpr bool EnableAssertions = true;
constexpr bool EnableShaderHotReload = true;
constexpr bool EnableGPUMarkers = true;
constexpr bool EnableShaderCompilerOptimization = false;

#elif defined(BUILD_DEV)

constexpr bool EnableValidationLayers = false;
constexpr bool EnableAssertions = false;
constexpr bool EnableShaderHotReload = true;
constexpr bool EnableGPUMarkers = true;
constexpr bool EnableShaderCompilerOptimization = true;

#elif defined(BUILD_RELEASE)

constexpr bool EnableValidationLayers = false;
constexpr bool EnableAssertions = false;
constexpr bool EnableShaderHotReload = false;
constexpr bool EnableGPUMarkers = false;
constexpr bool EnableShaderCompilerOptimization = true;

#else

#error "Unknown build configuration."

#endif

} // namespace BuildConfig