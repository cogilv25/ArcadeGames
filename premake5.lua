workspace "Arcade Games"
    startproject "Sudoku"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"
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

        

project "Sudoku"
    kind "ConsoleApp"
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
        "src/**.cpp",
        "src/**.hpp"
    }

    links
    {
        "GLFW",
        "GLM",
        "GLAD",
        "ImGui" 
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
include "lib/imgui.lua"