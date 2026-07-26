// Triangle example for vvhl

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

  return 0;
} 