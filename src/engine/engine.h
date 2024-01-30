#ifndef AG_ENGINE_H
#define AG_ENGINE_H

#include "platform.h"
#include "ui.h"

enum AG_SHADER
{
	TEXT
};

#ifndef AG_GAME_DLL

const Shader& getEngineShader(AG_SHADER shader);
void initializeEngine(Window&);
void destroyEngine();

#endif
#endif