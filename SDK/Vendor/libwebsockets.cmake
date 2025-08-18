include(FetchContent)
include(CheckFunctionExists)

FetchContent_Declare(
        libwebsockets
        GIT_REPOSITORY https://github.com/warmcat/libwebsockets.git
        GIT_TAG v4.4.1
)

set(LWS_WITH_MBEDTLS ON CACHE BOOL "" FORCE)
set(LWS_WITH_SSL ON CACHE BOOL "" FORCE)
set(LWS_WITH_STATIC ON CACHE BOOL "" FORCE)
set(LWS_WITH_SHARED OFF CACHE BOOL "" FORCE)
set(INSTALL_MBEDTLS_HEADERS OFF CACHE BOOL "" FORCE)
set(LWS_WITHOUT_EXTENSIONS ON CACHE BOOL "" FORCE)
set(LWS_WITHOUT_TESTAPPS ON CACHE BOOL "" FORCE) # don't build test apps

set(LWS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build examples
set(LWS_WITHOUT_CLIENT OFF CACHE BOOL "" FORCE) # build client part
set(LWS_WITHOUT_SERVER ON CACHE BOOL "" FORCE) # don't build server part

set(MBEDTLS_INCLUDE_DIRS "${mbedtls_SOURCE_DIR}/include" CACHE PATH "" FORCE)
set(LWS_MBEDTLS_INCLUDE_DIRS "${mbedtls_SOURCE_DIR}/include" CACHE PATH "" FORCE)

if (WIN32)
    set(MBEDTLS_LIBRARY "${mbedtls_BINARY_DIR}/library/mbedtls.lib;bcrypt" CACHE FILEPATH "" FORCE)
    set(MBEDX509_LIBRARY "${mbedtls_BINARY_DIR}/library/mbedx509.lib" CACHE FILEPATH "" FORCE)
    set(MBEDCRYPTO_LIBRARY "${mbedtls_BINARY_DIR}/library/mbedcrypto.lib" CACHE FILEPATH "" FORCE)
else ()
    set(MBEDTLS_LIBRARY "${mbedtls_BINARY_DIR}/library/libmbedtls.a" CACHE FILEPATH "" FORCE)
    set(MBEDX509_LIBRARY "${mbedtls_BINARY_DIR}/library/libmbedx509.a" CACHE FILEPATH "" FORCE)
    set(MBEDCRYPTO_LIBRARY "${mbedtls_BINARY_DIR}/library/libmbedcrypto.a" CACHE FILEPATH "" FORCE)
endif ()

set(CMAKE_REQUIRED_INCLUDES
        "${MBEDTLS_INCLUDE_DIRS}"
)
set(CMAKE_REQUIRED_LIBRARIES
        "${MBEDTLS_LIBRARY}"
        "${MBEDX509_LIBRARY}"
        "${MBEDCRYPTO_LIBRARY}"
        ${MBEDTLS_SYSTEM_LIBS}
)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

FetchContent_MakeAvailable(libwebsockets)

set_property(TARGET websockets PROPERTY INTERFACE_INCLUDE_DIRECTORIES "")

set(new_libs "")
set_target_properties(websockets PROPERTIES INTERFACE_LINK_LIBRARIES "${new_libs}")

target_link_libraries(SDK PUBLIC websockets)
target_include_directories(SDK PUBLIC "${libwebsockets_BINARY_DIR}/include")