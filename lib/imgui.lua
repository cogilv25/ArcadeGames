project "ImGui"
	kind "StaticLib"
	language "C"
	architecture "x64"

	staticruntime "On"
	
	includedirs
	{
		"imgui/",
		"glad/include",
		"glfw/include/"
	}

	files
	{
		"imgui/*.cpp",
		"imgui/backends/imgui_impl_glfw.cpp",
		"imgui/backends/imgui_impl_opengl3.cpp"
	}

	defines "IMGUI_IMPL_OPENGL_LOADER_GLAD"
    
	filter "system:linux"
		pic "On"

	filter "system:windows"

		defines "_CRT_SECURE_NO_WARNINGS"