file(GLOB_RECURSE EXT_CONFIG_FILES ${CMAKE_SOURCE_DIR}/Extensions/*/Config.cmake)

function(AddEnabledExtension ExtensionName)
    get_property(_current GLOBAL PROPERTY ENABLED_EXTENSIONS)
    list(APPEND _current ${ExtensionName})
    set_property(GLOBAL PROPERTY ENABLED_EXTENSIONS "${_current}")
endfunction()

function(RemoveEnabledExtension ExtensionName)
    get_property(_current GLOBAL PROPERTY ENABLED_EXTENSIONS)
    list(REMOVE_ITEM _current ${ExtensionName})
    set_property(GLOBAL PROPERTY ENABLED_EXTENSIONS "${_current}")
endfunction()

function(GetEnabledExtensions Result)
    get_property(_current GLOBAL PROPERTY ENABLED_EXTENSIONS)
    set(${Result} "${_current}" PARENT_SCOPE)
endfunction()

function(SetCurrentExtensionEnabled Value)
    set(ExtVarName "EXTENSION_${CurrentExtensionName}")
    set(${ExtVarName} ${Value} PARENT_SCOPE)
endfunction()

function(SetExtensionEnabled ExtensionName)
    message("Extension ${ExtensionName} is enabled!")
    AddEnabledExtension(${ExtensionName})
endfunction()

message("Begin configuring extensions")

list(REMOVE_DUPLICATES EXT_CONFIG_FILES)

foreach (EXT_CONFIG IN LISTS EXT_CONFIG_FILES)
    get_filename_component(ExtDir "${EXT_CONFIG}" DIRECTORY)
    get_filename_component(ExtName "${ExtDir}" NAME)

    set(CurrentExtensionName ${ExtName})

    include(${EXT_CONFIG})

    set(ExtVarName "EXTENSION_${ExtName}")

    if (DEFINED ${ExtVarName})
        if (${${ExtVarName}} STREQUAL "TRUE")
            message("Extension ${ExtName} is enabled!")
            AddEnabledExtension(${ExtName})
        endif ()
    endif ()
endforeach ()

message("End configuring extensions")