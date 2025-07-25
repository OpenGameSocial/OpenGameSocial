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

#ifdef PLATFORM_WINDOWS
    #define SCPRINTF(format, ...) _scprintf(format, __VA_ARGS__)
#else
    #define SCPRINTF(format, ...) SNPRINTF(nullptr, 0, format, __VA_ARGS__)
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