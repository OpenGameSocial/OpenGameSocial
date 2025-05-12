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
    cppdialect "C++20"

include("config.lua")
include("Platform/platforms.lua")

project "OpenGameSocial"
    language "C++"

    files {
        "Include/**.h", "Source/**.h", "Source/**.cpp",
        "Platform/%{cfg.platform}/**.h", "Platform/%{cfg.platform}/**.cpp"
    }

    includedirs { "Include", "Source", "Platform/%{cfg.platform}" }

    files { "Platform/%{cfg.platform}/**.mm" }

    ApplyConfiguration(true)

    ApplyPlatforms()

    filter {}

project "IntegrationTestingTool"
    language "C++"

    files { "Testing/**.h", "Testing/**.cpp" }

    includedirs { "Include", "Testing" }

    links { "OpenGameSocial" }

    ApplyConfiguration(false)

    ApplyPlatforms()

    filter {}

project "SolutionFiles"
	kind "utility"
	files { "premake5.lua", ".gitignore" }