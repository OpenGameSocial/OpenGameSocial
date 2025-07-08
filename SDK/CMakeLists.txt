cmake_minimum_required(VERSION 3.15)
project(OpenGameSocial)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(OGS_BUILD_SHARED "Build SDK as a shared library" ON)
option(OGS_BUILD_TESTING_TOOL "Build testing tool" ON)

add_subdirectory(Source)

if (OGS_BUILD_TESTING_TOOL)
    add_subdirectory(Testing)
endif ()