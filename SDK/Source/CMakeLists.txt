file(GLOB_RECURSE SDK_SOURCES *.cpp)

set(PLATFORM_SRC ${CMAKE_SOURCE_DIR}/Platform/${CMAKE_SYSTEM_NAME})
set(PLATFORM_BIN ${CMAKE_BINARY_DIR}/Platform/${CMAKE_SYSTEM_NAME})

file(GLOB_RECURSE PLATFORM_SDK_SOURCES ${PLATFORM_SRC}/Source/*.cpp)
list(APPEND SDK_SOURCES ${PLATFORM_SDK_SOURCES})

if (OGS_BUILD_SHARED)
    set(SDK_TYPE SHARED)
else ()
    set(SDK_TYPE STATIC)
endif ()

set(PLATFORM_CONFIG "${PLATFORM_SRC}/Config.cmake")

if (EXISTS "${PLATFORM_CONFIG}")
    include("${PLATFORM_CONFIG}")
endif ()

add_library(SDK ${SDK_TYPE} ${SDK_SOURCES})

set(OGS_LIBS
        mbedtls
        libwebsockets
        nlohmannjson
)

# Platform
add_subdirectory(
        ${PLATFORM_SRC}
        ${PLATFORM_BIN}
)

# Vendor
add_subdirectory(
        ${CMAKE_SOURCE_DIR}/Vendor
        ${CMAKE_BINARY_DIR}/Vendor
)

if (OGS_BUILD_SHARED)
    target_compile_definitions(SDK PUBLIC OGS_SHARED)
endif ()

target_compile_definitions(SDK PRIVATE OGS_EXPORTS)

target_include_directories(SDK PUBLIC ${CMAKE_SOURCE_DIR}/Include)
target_include_directories(SDK PRIVATE ${PLATFORM_SRC}/Source)
target_include_directories(SDK PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(SDK PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}"
)