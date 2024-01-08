workspace "Arcade Games"
    startproject "Launcher"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "bin/obj/%{cfg.buildcfg}"
    systemversion "latest"


    configurations
    {
        "Debug",
        "Release" 
    }

    flags "MultiProcessorCompile"

    filter "configurations:Debug"
        defines "_DEBUG"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "_RELEASE"
        runtime "Release"
        optimize "Speed"
        symbols "Off"
        flags "LinkTimeOptimization"

    filter "system:windows"
        ignoredefaultlibraries { "libcmt.lib" }

project "Engine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    architecture "x64"
        
    includedirs 
    { 
        "lib/glad/include/",
        "lib/glfw/include/",
        "lib/glm/",
        "lib/imgui/"
    }
    
    files
    {
        "src/engine/**.cpp",
        "src/engine/**.h"
    }

    links
    {
        "GLFW",
        "GLM",
        "GLAD"
    }

project "Launcher"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
	architecture "x64"

    includedirs 
    { 
        "lib/glm/",
        "src/engine/"
    }
    
    files
    {
        "src/launcher/**.cpp",
        "src/launcher/**.h"
    }

    links
    {
        "Engine"
    }

    defines
    {
        
    }

    filter "system:linux"
        links
        {
            "dl",
            "pthread"
        }

include "lib/glfw.lua"
include "lib/glad.lua"
include "lib/glm.lua"

include "src/games/sokoban.lua"
include "src/games/dancefloor.lua"