file(GLOB_RECURSE EXT_CONFIG_FILES ${CMAKE_SOURCE_DIR}/Extensions/*/*.cmake)

set(ENABLED_EXTENSIONS "")

foreach (EXT_CONFIG IN LISTS EXT_CONFIG_FILES)
    get_filename_component(ExtDir "${EXT_CONFIG}" DIRECTORY)
    get_filename_component(ExtName "${ExtDir}" NAME)

    include(${EXT_CONFIG})

    set(ExtVarName "EXTENSION_${ExtName}")

    if (ExtVarName AND DEFINED ${ExtVarName} AND ${${ExtVarName}})
        message("Extension ${ExtName} is enabled!")
        list(APPEND ENABLED_EXTENSIONS ${ExtName})
    endif ()
endforeach ()