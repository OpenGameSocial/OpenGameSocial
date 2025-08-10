include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 11.2.0
)

set(BUILD_SHARED_LIBS_BKP ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(fmt)

set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_BKP} CACHE BOOL "" FORCE)

target_link_libraries(SDK PUBLIC fmt)