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

Engine engine;
Shader ag_text_shader;
unsigned int ag_text_shader_position_location;
unsigned int ag_text_shader_colour_location;

void initializeEngine(Window& win)
{
	if (engine.alive)
		return;

	engine.win = win;


    if (!gladLoadGLLoader((GLADloadproc)getGLProcFunctionPointer(win)))
        std::cerr << "Fatal Error: GLAD failed to initialise" << std::endl;

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &engine.maxTextureSize);

    GL(glEnable(GL_BLEND));
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    GL(glEnable(GL_DEPTH_TEST));
    GL(glDepthFunc(GL_LESS));

    GL(glEnable(GL_CULL_FACE));
    

    GL(ag_text_shader = createShader("src/shaders/text.vs", "src/shaders/text.fs"));
    GL(glUseProgram(ag_text_shader.ID));
    GL(ag_text_shader_position_location = glGetUniformLocation(ag_text_shader.ID, "position"));
    GL(ag_text_shader_colour_location = glGetUniformLocation(ag_text_shader.ID, "colour"));
}

const Shader& getEngineShader(AG_SHADER shader)
{
    switch (shader)
    {
    case TEXT:
        return ag_text_shader;
    }
}

void destroyEngine()
{

}