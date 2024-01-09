pName = "Sokoban"

project(pName)
	kind "SharedLib"
	language "C++"
	architecture "x64" 

	includedirs
	{
		"../engine"
	}

	files 
	{
		pName.."/*.cpp",
		pName.."/*.h"
	}

	filter "system:windows"
		defines "_CRT_SECURE_NO_WARNINGS"
		prebuildcommands {"cd ../../bin/%{cfg.buildcfg} & if not exist games mkdir games"}
		postbuildcommands{ "cd ../../bin/%{cfg.buildcfg} & copy /D " .. pName .. ".dll games" }