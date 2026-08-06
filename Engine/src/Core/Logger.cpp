#include <vvhl/Core/Logger.hpp>

#include <spdlog/spdlog.h>

void Logger::Init() {
  spdlog::set_pattern("[%^%L%$] %v");

#if defined(BUILD_DEV) || defined(BUILD_DEBUG)
  spdlog::set_level(spdlog::level::debug);
#else
  spdlog::set_level(spdlog::level::info);
#endif
}
