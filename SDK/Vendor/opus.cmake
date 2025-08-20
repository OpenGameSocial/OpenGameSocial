include(FetchContent)

FetchContent_Declare(
        opus
        GIT_REPOSITORY https://github.com/xiph/opus.git
        GIT_TAG v1.5.2
)

FetchContent_MakeAvailable(opus)

target_link_libraries(SDK PUBLIC opus)