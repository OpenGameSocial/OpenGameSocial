GetEnabledExtensions(ENABLED_EXTENSIONS)

list(REMOVE_DUPLICATES ENABLED_EXTENSIONS)

foreach (EXTENSION IN LISTS ENABLED_EXTENSIONS)
    message("Adding enabled extension ${EXTENSION}")

    file(GLOB_RECURSE EXT_SOURCES ${CMAKE_SOURCE_DIR}/Extensions/${EXTENSION}/Source/*.cpp)

    target_sources(SDK PRIVATE ${EXT_SOURCES})
    target_include_directories(SDK PRIVATE ${CMAKE_SOURCE_DIR}/Extensions/${EXTENSION}/Source)
    target_compile_definitions(SDK PRIVATE "WITH_${EXTENSION}")

    if (EXISTS "${CMAKE_SOURCE_DIR}/Extensions/${EXTENSION}/CMakeLists.txt")
        add_subdirectory(${CMAKE_SOURCE_DIR}/Extensions/${EXTENSION})
    endif ()
endforeach ()