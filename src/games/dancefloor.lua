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

	filter "system:windows"
		defines "_CRT_SECURE_NO_WARNINGS"