#include "platform.h"
#include "binary_interface.h"
#include <GLFW/glfw3.h>
#include <iostream>

void createWindow(Window& win)
{
	if (!glfwInit())
		std::cerr << "Fatal Error: GLFW failed to initialise" << std::endl;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, win.glVersion[0]);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, win.glVersion[1]);
	glfwWindowHint(GLFW_DECORATED, !win.borderless);

	if (win.glCoreProfile)
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	else
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);


	GLFWmonitor* fs = 0;
	if (win.fullscreen)
		fs = glfwGetPrimaryMonitor();


	win.glfwRef = glfwCreateWindow(win.width, win.height, win.title, 0, 0);
	if (win.glfwRef == 0)
	{
		std::cerr << "Error creating GLFW window\n";
		return;
	}

	glfwMakeContextCurrent((GLFWwindow*)win.glfwRef);
}

void destroyWindow(Window& win)
{
	glfwDestroyWindow((GLFWwindow*)win.glfwRef);
	glfwTerminate();
}

void* getGLProcFunctionPointer(Window& win)
{
	return (win.glfwRef == 0) ? 0 : glfwGetProcAddress;
}

void updateWindow(Window& win)
{
	glfwSwapBuffers((GLFWwindow*)win.glfwRef);
	glfwPollEvents();

	if (glfwWindowShouldClose((GLFWwindow*)win.glfwRef))
		win.shouldClose = true;
}

unsigned long long getClock()
{
	return glfwGetTimerValue();
}

double getTime()
{
	return glfwGetTime();
}

bool getKeyDown(Window& win, int key)
{
	return glfwGetKey((GLFWwindow*)win.glfwRef, key) == GLFW_PRESS;
}

CursorPos getCursorPosition(Window& win)
{
	CursorPos pos = { 0,0 };
	glfwGetCursorPos((GLFWwindow*)win.glfwRef, &pos.x, &pos.y);
	pos.x /= (double)win.width / 2.0;
	pos.y /= (double)win.height / 2.0;
	return pos;
}

bool getMouseButtonDown(Window& win, bool left)
{
	return glfwGetMouseButton((GLFWwindow*)win.glfwRef,left? GLFW_MOUSE_BUTTON_LEFT: GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS;
}
