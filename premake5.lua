include "dependencies.lua"

workspace "Luminx"
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
	include "LuminxEngine/vendor/GLFW"
	include "LuminxEngine/vendor/Glad"
	include "LuminxEngine/vendor/imgui"
group ""

group "Apps"
	group "Apps/Basic"
		include "Apps/TriangleApp"
		include "Apps/LoadModelApp"
		include "Apps/TextureApp"
		include "Apps/CameraApp"
		include "Apps/BasicLightingApp"
	group "Apps/Lighting"
		include "Apps/BlinnPhongApp"
group ""

include "LuminxEngine"
include "Sandbox"