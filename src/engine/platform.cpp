#include "platform.h"
#include "glad/glad.h"
#include "utilities.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

static void frameBufferSizeChanged(GLFWwindow* winRef, int width, int height)
{
	GL(glViewport(0, 0, width, height));
}

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
	glfwSetFramebufferSizeCallback((GLFWwindow*)win.glfwRef, frameBufferSizeChanged);
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


// Returns a pointer to a char array containing the contents of the file at the path provided.
// If no file is found or there is another error, returns NULL and reports failure through fprintf.
char* loadFileAsCharArray(const char* path)
{
	FILE* file = NULL;
	char* fileData = NULL;
	long fileSize;

	// Open the file in binary mode
	errno_t err = fopen_s(&file, path, "rb");
	if (err != 0 || file == NULL)
	{
		fprintf(stderr, "Unable to find or open the requested file: %s\n", path);
		return NULL;
	}

	// Determine the file size
	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Allocate memory for the file data
	fileData = (char*)malloc(fileSize + 1);
	if (fileData == NULL)
	{
		fprintf(stderr, "Memory allocation failed\n");
		fclose(file);
		return NULL;
	}

	// Read the file content into the allocated memory
	size_t bytesRead = fread(fileData, 1, fileSize, file);
	fclose(file);

	if (bytesRead != fileSize)
	{
		fprintf(stderr, "Error reading file: %s\n", path);
		free(fileData);
		return NULL;
	}

	// Null-terminate the char array
	fileData[fileSize] = '\0';

	return fileData;
}
