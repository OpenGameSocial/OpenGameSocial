file(GLOB_RECURSE SDK_SOURCES *.cpp)

file(GLOB_RECURSE PLATFORM_SDK_SOURCES ${CMAKE_SOURCE_DIR}/Platform/${CMAKE_SYSTEM_NAME}/Source/*.cpp)
list(APPEND SDK_SOURCES ${PLATFORM_SDK_SOURCES})

if (OGS_BUILD_SHARED)
    set(SDK_TYPE SHARED)
else ()
    set(SDK_TYPE STATIC)
endif ()

add_library(SDK ${SDK_TYPE} ${SDK_SOURCES})

# Platform
add_subdirectory(
        ${CMAKE_SOURCE_DIR}/Platform/${CMAKE_SYSTEM_NAME}
        ${CMAKE_BINARY_DIR}/Platform/${CMAKE_SYSTEM_NAME}
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
target_include_directories(SDK PRIVATE ${CMAKE_SOURCE_DIR}/Platform/${CMAKE_SYSTEM_NAME}/Source)
target_include_directories(SDK PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(SDK PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}"
)