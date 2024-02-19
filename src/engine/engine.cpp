#include "engine.h"
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

Shader ag_shaders[1];
Font ag_fonts[1];

Engine engine;
unsigned int ag_text_shader_position_location;
unsigned int ag_text_shader_colour_location;

void initializeEngine(Window& win)
{
	if (engine.alive)
		return;

	engine.win = win;


    if (!gladLoadGLLoader((GLADloadproc)getGLProcFunctionPointer(win)))
        fprintf(stderr, "Fatal Error: GLAD failed to initialise\n");

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &engine.maxTextureSize);

    GL(glEnable(GL_BLEND));
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    GL(glEnable(GL_DEPTH_TEST));
    GL(glDepthFunc(GL_LESS));

    GL(glEnable(GL_CULL_FACE));
    
    ag_fonts[0] = loadFont("assets/Spectral.ttf");

    GL(ag_shaders[0] = createShader("src/shaders/text.vs", "src/shaders/text.fs"));

    GL(glUseProgram(ag_shaders[0].ID));
    GL(ag_text_shader_position_location = glGetUniformLocation(ag_shaders[0].ID, "position"));
    GL(ag_text_shader_colour_location = glGetUniformLocation(ag_shaders[0].ID, "colour"));
}

Shader& getEngineShader(AG_SHADER shader)
{
    switch (shader)
    {
    case TEXT:
        return ag_shaders[0];
    }
}

Font& getEngineFont(AG_FONT font)
{
    switch (font)
    {
    case SPECTRAL:
        return ag_fonts[0];
    }
}


void destroyEngine()
{

}