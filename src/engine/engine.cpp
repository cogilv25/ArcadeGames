#include "engine.h"
#include "binary_interface.h"
#include "utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

struct Engine
{
	Window win;
	bool alive = false;
};

Engine engine;

void initializeEngine(Window& win)
{
	if (engine.alive)
		return;

	engine.win = win;


    if (!gladLoadGLLoader((GLADloadproc)getGLProcFunctionPointer(win)))
        std::cerr << "Fatal Error: GLAD failed to initialise" << std::endl;

    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL(glEnable(GL_BLEND));
    GL(glEnable(GL_DEPTH_TEST));


    GL(glEnable(GL_CULL_FACE));
}

void destroyEngine()
{

}