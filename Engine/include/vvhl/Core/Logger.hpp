#pragma once
#include <spdlog/spdlog.h>

class Logger{
public:
    static void init();
};

// Debug
#define LOGD(...) spdlog::debug(__VA_ARGS__);

// Info
#define LOGI(...) spdlog::info(__VA_ARGS__);

// Warning
#define LOGW(...) spdlog::warn(__VA_ARGS__);

// Error
#define LOGE(...) spdlog::error(__VA_ARGS__);

// Critical
#define LOGC(...) spdlog::critical(__VA_ARGS__);

// Flush
#define LOGDUMP() spdlog::dump_backtrace();