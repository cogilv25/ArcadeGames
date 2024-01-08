project "GLAD"
	kind "StaticLib"
	language "C"
	architecture "x64"

	staticruntime "On"

    includedirs "glad/include/"

	files "glad/src/glad.c"

	filter "system:linux"
		pic "On"

	filter "system:windows"

		defines "_CRT_SECURE_NO_WARNINGS"