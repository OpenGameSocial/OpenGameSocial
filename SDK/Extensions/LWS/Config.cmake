if (CMAKE_SYSTEM_NAME STREQUAL "Windows" OR
        CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR
        CMAKE_SYSTEM_NAME STREQUAL "Linux")
    SetExtensionEnabled(TRUE)
else ()
    SetExtensionEnabled(FALSE)
endif ()
