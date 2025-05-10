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

#ifdef PLATFORM_WINDOWS
    #define DEBUG_BREAK __debugbreak
#else
    #define DEBUG_BREAK __builtin_debugtrap
#endif

#ifdef PLATFORM_WINDOWS
    #define SSCANF(buffer, format, ...) sscanf_s(buffer, format, __VA_ARGS__)
    #define SNPRINTF(buffer, size, format, ...) _snprintf_s(buffer, size, _TRUNCATE, format, __VA_ARGS__)
#else
    #define SSCANF(buffer, format, ...) sscanf(buffer, format, __VA_ARGS__)
    #define SNPRINTF(buffer, size, format, ...) snprintf(buffer, size, format, __VA_ARGS__)
#endif

#define CHECK(expr)