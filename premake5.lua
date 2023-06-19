include "dependencies.lua"

workspace "Luminix"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"
outputbindir = "build/bin/%{cfg.buildcfg}-%{cfg.architecture}"
outputintdir = "build/int/%{cfg.buildcfg}-%{cfg.architecture}"

group "Dependencies"
	include "LuminixEngine/vendor/GLFW"
	include "LuminixEngine/vendor/Glad"
	include "LuminixEngine/vendor/imgui"
group ""

include "LuminixEngine"
include "Sandbox"