add_executable(TestingTool
        main.cpp
)

target_link_libraries(TestingTool PRIVATE SDK)

target_include_directories(TestingTool PRIVATE ${CMAKE_SOURCE_DIR}/Include)

set_target_properties(TestingTool PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}"
)