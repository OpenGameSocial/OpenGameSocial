include(FetchContent)

FetchContent_Declare(
        libwebsockets
        GIT_REPOSITORY https://github.com/warmcat/libwebsockets.git
        GIT_TAG v4.3.5
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

FetchContent_MakeAvailable(libwebsockets)

if (NOT APPLE)
#    target_compile_definitions(websockets PUBLIC LWS_HAVE_MBEDTLS_AUTH_KEY_ID)
#    target_compile_definitions(websockets PUBLIC LWS_HAVE_mbedtls_ssl_conf_alpn_protocols)
#    target_compile_definitions(websockets PUBLIC LWS_HAVE_mbedtls_ssl_get_alpn_protocol)
#    target_compile_definitions(websockets PUBLIC LWS_HAVE_mbedtls_ssl_conf_sni)
#    target_compile_definitions(websockets PUBLIC LWS_HAVE_mbedtls_ssl_set_hs_ca_chain)
#    target_compile_definitions(websockets PUBLIC LWS_HAVE_mbedtls_ssl_set_hs_own_cert)
#    target_compile_definitions(websockets PUBLIC LWS_HAVE_mbedtls_ssl_set_hs_authmode)
#    target_compile_definitions(websockets PUBLIC LWS_HAVE_mbedtls_net_init)
#    target_compile_definitions(websockets PUBLIC LWS_HAVE_mbedtls_x509_crt_parse_file) # some embedded may lack filesystem
#    target_compile_definitions(websockets PUBLIC LWS_HAVE_mbedtls_md_setup) # not on xenial 2.2
#    target_compile_definitions(websockets PUBLIC LWS_HAVE_mbedtls_rsa_complete) # not on xenial 2.2
#    target_compile_definitions(websockets PUBLIC LWS_HAVE_mbedtls_internal_aes_encrypt)
#    target_compile_definitions(websockets PUBLIC LWS_HAVE_mbedtls_ssl_set_verify)
endif ()

set_property(TARGET websockets PROPERTY INTERFACE_INCLUDE_DIRECTORIES "")

set(new_libs "")
set_target_properties(websockets PROPERTIES INTERFACE_LINK_LIBRARIES "${new_libs}")

target_link_libraries(SDK PUBLIC websockets)
target_include_directories(SDK PUBLIC "${libwebsockets_BINARY_DIR}/include")