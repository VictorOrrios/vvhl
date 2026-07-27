
#ifdef BUILD_RELEASE
    #define ASSERT(x)
#else
    #define ASSERT(x) assert(x);
#endif