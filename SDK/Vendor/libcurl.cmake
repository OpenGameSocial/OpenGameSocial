include(FetchContent)

FetchContent_Declare(
        curl
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG curl-8_14_1
)

set(CURL_USE_MBEDTLS ON CACHE BOOL "" FORCE)
set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
set(ENABLE_CURL_MANUAL OFF CACHE BOOL "" FORCE)
set(BUILD_LIBCURL_DOCS OFF CACHE BOOL "" FORCE)

set(MBEDTLS_INCLUDE_DIRS "${mbedtls_SOURCE_DIR}/include" CACHE PATH "" FORCE)

FetchContent_MakeAvailable(curl)

target_link_libraries(SDK PUBLIC libcurl)