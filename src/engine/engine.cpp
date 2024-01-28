#include "engine.h"
#include "binary_interface.h"
#include "utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

struct Engine
{
	Window win;
	bool alive = false;
    int maxTextureSize;
};

Engine engine;

void initializeEngine(Window& win)
{
	if (engine.alive)
		return;

	engine.win = win;


    if (!gladLoadGLLoader((GLADloadproc)getGLProcFunctionPointer(win)))
        std::cerr << "Fatal Error: GLAD failed to initialise" << std::endl;

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &engine.maxTextureSize);

    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    GL(glEnable(GL_BLEND));
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL(glEnable(GL_DEPTH_TEST));
    GL(glDepthFunc(GL_LESS));

    GL(glEnable(GL_CULL_FACE));
    
}

void destroyEngine()
{

}