project "SimpleFogApp"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "Off"

	targetdir ("%{wks.location}/" .. outputbindir .. "/%{prj.name}")
	objdir ("%{wks.location}/" .. outputintdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/LuminxEngine/src",
		"%{wks.location}/LuminxEngine/vendor/spdlog/include",
		"%{wks.location}/LuminxEngine/vendor/glm",
		"%{wks.location}/LuminxEngine/vendor/imgui",
	}

	links
	{
		"LuminxEngine"
	}

	postbuildcommands
	{
		"{COPYDIR} \"%{prj.location}assets\" \"%{wks.location}/" .. outputbindir .. "/%{prj.name}/assets" .. "/\""
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"LUM_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "LUM_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "LUM_RELEASE"
		runtime "Release"
		optimize "On"
		symbols "On"

	filter "configurations:Dist"
		kind "WindowedApp"
		defines "LUM_DIST"
		runtime "Release"
		optimize "On"
		symbols "Off"