#ifndef AG_ENGINE_H
#define AG_ENGINE_H

#include "platform.h"
#include "ui.h"



enum AG_SHADER
{
	TEXT
};

enum AG_FONT
{
	SPECTRAL
};

#ifndef AG_GAME_DLL

Shader& getEngineShader(AG_SHADER shader);
Font& getEngineFont(AG_FONT);
void initializeEngine(Window&);
void destroyEngine();

#endif
#endif