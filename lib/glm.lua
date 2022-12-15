project "GLM"
	kind "StaticLib"
	language "C"
	architecture "x64"

	staticruntime "On"
	
	includedirs "glm/"

	files "glm/glm/**"
    
	filter "system:linux"
		pic "On"

	filter "system:windows"

		defines "_CRT_SECURE_NO_WARNINGS"