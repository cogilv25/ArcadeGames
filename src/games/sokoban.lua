project "Sokoban"
	kind "SharedLib"
	language "C++"
	architecture "x64" 

	includedirs
	{
		"../engine"
	}

	files 
	{
		"sokoban/*.cpp",
		"sokoban/*.h"
	}

	filter "system:windows"
		defines "_CRT_SECURE_NO_WARNINGS"