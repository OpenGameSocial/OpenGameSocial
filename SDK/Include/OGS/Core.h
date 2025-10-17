#pragma once


#if defined(_WIN64)
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
    #define DEBUG_BREAK() __debugbreak()
#elif defined(PLATFORM_MACOS)
    #define DEBUG_BREAK() __builtin_debugtrap()
#elif defined(PLATFORM_LINUX) && defined(__GNUC__)
    #include <signal.h>
    #define DEBUG_BREAK() raise(SIGTRAP)
#else
    #include <signal.h>
    #define DEBUG_BREAK() raise(SIGTRAP)
#endif

#define CHECK(expr)

#define INVALID_SUBSCRIPTION_ID (0xFFFFFFFFFFFFFFFF)

typedef uint64_t OGS_SubscriptionId;

enum OGS_ELogLevel
{
    OGS_Verbose = 0,
    OGS_Info = 1,
    OGS_Warning = 2,
    OGS_Error = 3,
    OGS_Critical = 4
};

inline const char* LevelToString(OGS_ELogLevel Level)
{
    switch (Level)
    {
    case OGS_Verbose:
        return "Verbose";
    case OGS_Info:
        return "Info";
    case OGS_Warning:
        return "Warning";
    case OGS_Error:
        return "Error";
    case OGS_Critical:
        return "Critical";
    }

    DEBUG_BREAK();
    return "Unknown";
}