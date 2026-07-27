#include "vvhl/Core/Logger.hpp"
#include <vvhl/vvhl.hpp>


void bar(){
    
    Logger log;
    log.Init();

    fmt::println("Hello! {}",Config::MaxFramesInFlight);

    LOGD("Debug msgs")
    LOGI("Info msgs")
    LOGW("Warn msgs")
    LOGE("Error msgs")
    LOGC("Critical msgs")
}