project "Smoothing"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "On"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

	defines{ "_CRT_SECURE_NO_WARNINGS" }
	linkoptions { "-IGNORE:4099" }

	files { 
		"../Core/src/common.h",
		"src/**.c", 
		"src/**.h", 
		"src/**.cpp", 
		"premake5.lua" 
	}

	includedirs
	{
		"../Core/src",
		"src",
		"../vendor/raylib/src",
		"../vendor/raylib/src/external/glfw/include",
		"../vendor/raygui/src"
	}
	-------------------------------------------------------------------
	-- raylib
	links { "raylib" }

	filter "system:windows"
		systemversion "latest"
		libdirs {"../vendor/raylib/bin/" .. outputdir .. "/raylib"}
		dependson { "raylib" }
		links { "raylib.lib" }
	-------------------------------------------------------------------