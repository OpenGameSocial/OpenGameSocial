file(GLOB_RECURSE EXT_CONFIG_FILES ${CMAKE_SOURCE_DIR}/Extensions/*/Config.cmake)

set(ENABLED_EXTENSIONS "")

function(SetExtensionEnabled Value)
    set(ExtVarName "EXTENSION_${CurrentExtensionName}")
    set(${ExtVarName} ${Value} PARENT_SCOPE)
endfunction()

message("Begin configuring extensions")

foreach (EXT_CONFIG IN LISTS EXT_CONFIG_FILES)
    get_filename_component(ExtDir "${EXT_CONFIG}" DIRECTORY)
    get_filename_component(ExtName "${ExtDir}" NAME)

    set(CurrentExtensionName ${ExtName})

    include(${EXT_CONFIG})

    set(ExtVarName "EXTENSION_${ExtName}")
    message(${ExtVarName})
    message(${${ExtVarName}})

    if (ExtVarName AND DEFINED ${ExtVarName} AND ${${ExtVarName}})
        message("Extension ${ExtName} is enabled!")
        list(APPEND ENABLED_EXTENSIONS ${ExtName})
    endif ()
endforeach ()

message("End configuring extensions")