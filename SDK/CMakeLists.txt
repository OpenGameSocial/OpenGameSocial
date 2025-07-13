cmake_minimum_required(VERSION 3.15)
project(OpenGameSocial)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(OGS_BUILD_SHARED "Build SDK as a shared library" ON)
option(OGS_BUILD_TESTING_TOOL "Build testing tool" ON)

get_filename_component(CODEGEN_TOOL_PROJECT
        "${CMAKE_SOURCE_DIR}/../CodegenTool/CodegenTool/CodegenTool.csproj"
        ABSOLUTE)

execute_process(
        COMMAND dotnet run --project ${CODEGEN_TOOL_PROJECT} -- ${CMAKE_SOURCE_DIR} ${CMAKE_SYSTEM_NAME} "${CMAKE_BINARY_DIR}/Generated"
        RESULT_VARIABLE GEN_RESULT
)

if (NOT GEN_RESULT EQUAL 0)
    message(FATAL_ERROR "Code generation failed: ${GEN_RESULT}")
endif()

add_subdirectory(Source)

if (OGS_BUILD_TESTING_TOOL)
    add_subdirectory(Testing)
endif ()