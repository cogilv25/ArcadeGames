pName = "DanceFloor"

project "DanceFloor"
	kind "SharedLib"
	language "C++"
	architecture "x64" 

	includedirs
	{
		"../engine"
	}

	files 
	{
		"dancefloor/*.cpp",
		"dancefloor/*.h"
	}

	filter {"system:windows", "configurations:not Production"}
		defines "_CRT_SECURE_NO_WARNINGS"
		prebuildcommands {"cd ../../bin/%{cfg.buildcfg} & if not exist games mkdir games"}
		postbuildcommands{ "cd ../../bin/%{cfg.buildcfg} & copy /D " .. pName .. ".dll games" }