include(FetchContent)

FetchContent_Declare(
        date
        GIT_REPOSITORY https://github.com/HowardHinnant/date.git
        GIT_TAG v3.0.4
)

set(BUILD_SHARED_LIBS_BKP ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(date)

set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_BKP} CACHE BOOL "" FORCE)

target_link_libraries(SDK PUBLIC date)