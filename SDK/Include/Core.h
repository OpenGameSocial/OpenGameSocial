#pragma once


#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define PLATFORM_MACOS
#elif defined(__linux__)
    #define PLATFORM_LINUX
#else
    #error "Unsupported platform!"
#endif

#ifdef OGS_SHARED
#if defined(PLATFORM_WINDOWS)
    #ifdef OGS_EXPORTS
        #define OGS_API __declspec(dllexport)
    #else
        #define OGS_API __declspec(dllimport)
    #endif
#elif defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX)
    #ifdef OGS_EXPORTS
        #define OGS_API __attribute__((visibility("default")))
    #else
        #define OGS_API
    #endif
#else
    #define OGS_API
#endif
#else
    #define OGS_API
#endif

OGS_API void Testfunc();
