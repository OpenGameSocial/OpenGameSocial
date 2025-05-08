workspace "SDK"
    configurations {
        "DebugShared",
        "DebugStatic",

        "ReleaseShared",
        "ReleaseStatic"
    }

    platforms { "Win64", "MacOSX" }

    targetdir ("Build/" .. os.target() .. "/%{cfg.buildcfg}")
    objdir ("Intermediate/" .. os.target())

function ApplyDebugSettings()
    defines { "DEBUG" }
    symbols "On"
end

function ApplyReleaseSettings()
    defines { "NDEBUG" }
    optimize "On"
end

function ApplyConfiguration(libTarget)
    local sharedKind = libTarget and "SharedLib" or "ConsoleApp"
    local staticKind = libTarget and "StaticLib" or "ConsoleApp"

    filter "configurations:DebugShared"
        kind (sharedKind)
        defines { "OGS_SHARED" }

        if libTarget then
            defines { "OGS_EXPORTS" }
        end

        ApplyDebugSettings()

    filter "configurations:DebugStatic"
        defines { "OGS_STATIC" }
        kind (staticKind)
        ApplyDebugSettings()

    filter "configurations:ReleaseShared"
        defines { "OGS_SHARED" }
        kind (sharedKind)

        if libTarget then
            defines { "OGS_EXPORTS" }
        end

        ApplyReleaseSettings()

    filter "configurations:ReleaseStatic"
        defines { "OGS_STATIC" }
        kind (staticKind)
        ApplyReleaseSettings()

    filter {}
end

function ApplyPlatforms()
    filter { "platforms:Win64" }
        system "Windows"
        architecture "x86_64"

    filter { "platforms:MacOSX" }
        system "macosx"
        architecture "ARM64"

    filter {}
end

project "OpenGameSocial"
    language "C++"

    files { "Include/**.h", "Source/**.h", "Source/**.cpp" }

    includedirs { "Include", "Source" }

    ApplyConfiguration(true)

    ApplyPlatforms()

project "IntegrationTestingTool"
    language "C++"

    files { "Testing/**.h", "Testing/**.cpp" }

    includedirs { "Include", "Testing" }

    links { "OpenGameSocial" }

    ApplyConfiguration(false)

    ApplyPlatforms()