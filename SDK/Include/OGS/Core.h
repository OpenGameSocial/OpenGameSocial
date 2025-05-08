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

#ifdef __cplusplus
    #define OGS_EXTERN_C extern "C"
#else
    #define OGS_EXTERN_C
#endif

#ifdef OGS_SHARED
#if defined(PLATFORM_WINDOWS)
    #ifdef OGS_EXPORTS
        #define OGS_API OGS_EXTERN_C __declspec(dllexport)
    #else
        #define OGS_API OGS_EXTERN_C __declspec(dllimport)
    #endif
#elif defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX)
    #ifdef OGS_EXPORTS
        #define OGS_API OGS_EXTERN_C __attribute__((visibility("default")))
    #else
        #define OGS_API OGS_EXTERN_C
    #endif
#else
    #define OGS_API OGS_EXTERN_C
#endif
#else
    #define OGS_API
#endif
