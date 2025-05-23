project "libcrypto"
    kind "StaticLib"
    language "C"
    targetdir "build/%{cfg.buildcfg}"

    files {
        "OpenSSL/crypto/**/*.c",
        "OpenSSL/crypto/**/*.h",
    }

    includedirs {
        "OpenSSL",
        "OpenSSL/include"
    }

    filter "system:windows"
        defines { "WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS" }

    filter "configurations:DebugShared"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:DebugStatic"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:ReleaseShared"
        defines { "NDEBUG" }
        optimize "On"

    filter "configurations:ReleaseStatic"
        defines { "NDEBUG" }
        optimize "On"

