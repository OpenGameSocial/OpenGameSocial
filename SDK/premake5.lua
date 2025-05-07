workspace "SDK"
	configurations {
		"DebugShared",
		"DebugStatic",
		
		"ReleaseShared",
		"ReleaseStatic"
	}

	platforms { "Win64", "MacOSX" }
	
function ApplyDebugSettings()
	defines { "DEBUG" }
	symbols "On"
end

function ApplyReleaseSettings()
	defines { "NDEBUG" }
	optimize "On"
end

function ApplyConfiguration()
	filter "configurations:DebugShared"
		kind "SharedLib"
		ApplyDebugSettings()

	filter "configurations:DebugStatic"
		kind "StaticLib"
		ApplyDebugSettings()

	filter "configurations:ReleaseShared"
		kind "SharedLib"
		ApplyReleaseSettings()

	filter "configurations:ReleaseStatic"
		kind "StaticLib"
		ApplyReleaseSettings()
end

project "OpenGameSocial"
	language "C++"
	targetdir ("Build/" .. os.target() .. "/%{cfg.buildcfg}")
	objdir ("Intermediate/" .. os.target())

	files { "**.h", "**.cpp" }

	ApplyConfiguration()

	filter { "platforms:Win64" }
		system "Windows"
		architecture "x86_64"

	filter { "platforms:MacOSX" }
		system "macosx"
		architecture "ARM64"