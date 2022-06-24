workspace "SpringItOn"
	configurations { "Debug", "Release", "Dist" }
	platforms "x64"
	architecture "x86_64"
	startproject "Inertialization"

	filter "configurations:Debug"
		defines { "DEBUG" }
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines { "NDEBUG" }
		runtime "Release"
		optimize "on"
		symbols "off"

	filter "configurations:Dist"
		defines { "NDEBUG" }
		runtime "Release"
		optimize "on"
		symbols "off"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	----------------------------------------------------------------
	-- Projects
	group "Dependencies"
		include "vendor/raylib"
	group ""
	
	include "Damper"
	include "SpringDamper"
	include "Inertialization"
	include "Smoothing"