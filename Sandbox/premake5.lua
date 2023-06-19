project "Sandbox"
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
		"%{wks.location}/LuminixEngine/src",
		"%{wks.location}/LuminixEngine/vendor/spdlog/include",
		"%{wks.location}/LuminixEngine/vendor/glm",
		"%{wks.location}/LuminixEngine/vendor/imgui",
	}

	links
	{
		"LuminixEngine"
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