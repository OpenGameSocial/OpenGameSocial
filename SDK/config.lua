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
    filter { "action:vs*" }
        buildoptions { "/MP" }

    filter { "platforms:Win64" }
        system "Windows"
        architecture "x86_64"

        filter { "platforms:Win64", "configurations:DebugShared" }
            buildoptions { "/MTd" }

        filter { "platforms:Win64", "configurations:DebugStatic" }
            buildoptions { "/MTd" }

        filter { "platforms:Win64", "configurations:ReleaseShared" }
            buildoptions { "/MT" }

        filter { "platforms:Win64", "configurations:ReleaseStatic" }
            buildoptions { "/MT" }

    filter { "platforms:MacOSX" }
        system "macosx"
        architecture "ARM64"
        links { "Foundation.framework", "objc" }

    filter {}
end