// Triangle example for vvhl

#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include <vvhl/vvhl.hpp>
#include <vvhl/Core/Config.hpp>
#include <vvhl/Core/foo.hpp>

#include <iostream>
using namespace std;


int main() {
#ifdef BUILD_DEBUG

    cout << "Debug build defined";

#elif defined(BUILD_DEV)

    cout << "Dev build defined";

#elif defined(BUILD_RELEASE)

    cout << "Release build defined";

#else

  cout << "Error no build defined!";

#endif

    bar();

    LOGD("Debug msgs 2")
    LOGI("Info msgs 2")
    LOGW("Warn msgs 2")
    LOGE("Error msgs 2")
    LOGC("Critical msgs 2")

  return 0;
} 