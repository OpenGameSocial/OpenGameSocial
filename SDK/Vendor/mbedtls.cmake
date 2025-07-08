include(FetchContent)

FetchContent_Declare(
        mbedtls
        GIT_REPOSITORY https://github.com/ARMmbed/mbedtls.git
        GIT_TAG mbedtls-3.6.4
        OVERRIDE_FIND_PACKAGE
)

set(ENABLE_PROGRAMS OFF CACHE BOOL "" FORCE)
set(ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(USE_STATIC_MBEDTLS_LIBRARY ON CACHE BOOL "" FORCE)
set(USE_SHARED_MBEDTLS_LIBRARY OFF CACHE BOOL "" FORCE)
set(MSVC_STATIC_RUNTIME ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(mbedtls)

target_link_libraries(SDK
        PUBLIC
        MbedTLS::mbedcrypto MbedTLS::mbedx509 MbedTLS::mbedtls
)

target_include_directories(SDK PUBLIC ${mbedtls_SOURCE_DIR}/include)
