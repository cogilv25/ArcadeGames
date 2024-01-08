#include "render.h"
#include "glad/glad.h"
#include "utilities.h"
#include "binary_interface.h"
#include <memory>
#include <iostream>

extern std::shared_ptr<char> loadFileAsCharArray(const char* path);

Shader createShader(const char* vsPath, const char* fsPath)
{
	Shader s {0, true};
	
	// Get rid of this awful shared pointer stuff!
	auto vertexShaderSource = loadFileAsCharArray(vsPath);
	auto fragmentShaderSource = loadFileAsCharArray(fsPath);
	auto vertexShaderSourcePointer = vertexShaderSource.get();
	auto fragmentShaderSourcePointer = fragmentShaderSource.get();

	if (vertexShaderSource == 0 || fragmentShaderSource == 0)
	{
		std::cerr << "Shader Source Path Doesn't Exist\n";
		s.valid = false;
		return s;
	}

	int result;
	char info[512];

	unsigned int vert = glCreateShader(GL_VERTEX_SHADER), frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vert, 1, &vertexShaderSourcePointer, 0);
	glCompileShader(vert);

	glGetShaderiv(vert, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vert, 512, 0, info);
		std::cerr << "Vertex Shader Couldn't Compile:\n" << info << "\n\n";
		s.valid = false;
	}

	glShaderSource(frag, 1, &fragmentShaderSourcePointer, 0);
	glCompileShader(frag);

	glGetShaderiv(frag, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(frag, 512, 0, info);
		std::cerr << "Fragment Shader Couldn't Compile:\n" << info << "\n\n";
		s.valid = false;
	}

	s.ID = glCreateProgram();
	glAttachShader(s.ID, vert);
	glAttachShader(s.ID, frag);
	glLinkProgram(s.ID);

	glGetProgramiv(s.ID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(s.ID, 512, 0, info);
		std::cout << "Shader Program couldn't Link:\n" << info << "\n\n";
		s.valid = false;
	}
	else
		IFDBG(std::cout << "Shader program successfully compiled!\n";)

	glDeleteShader(vert);
	glDeleteShader(frag);
    return s;
}

void useShader(Shader& s)
{
	GL(glUseProgram(s.ID));
}

void clearFrameBuffer()
{
	GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

Geometry createGeometry(float* vertices, unsigned int * indices, unsigned int * compPerLoc, unsigned int vertexCount, unsigned int indexCount, unsigned int locCount, unsigned int stride)
{
	Geometry geom{ 0, 0, 0, vertices, indices, vertexCount, indexCount, stride };

	//Create Vertex Array, Vertex Buffer and Index Buffer
	GL(glGenVertexArrays(1, &geom.vao));
	GL(glBindVertexArray(geom.vao));

	GL(glGenBuffers(1, &geom.vbo));
	GL(glBindBuffer(GL_ARRAY_BUFFER, geom.vbo));
	GL(glBufferData(GL_ARRAY_BUFFER, sizeof(float)*stride*vertexCount, geom.vertices, GL_DYNAMIC_DRAW));

	unsigned int offset = 0;
	for (int i = 0; i < locCount; i++)
	{
		GL(glVertexAttribPointer(i, compPerLoc[i], GL_FLOAT, GL_FALSE, sizeof(float)*stride, (void*)(sizeof(float)*offset)));
		GL(glEnableVertexAttribArray(i));
		offset += compPerLoc[i];
	}

	GL(glGenBuffers(1, &geom.ibo));
	GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geom.ibo));
	GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexCount, indices, GL_DYNAMIC_DRAW));

	return geom;
}

void drawGeometry(Geometry& geom)
{
	GL(glBindVertexArray(geom.vao));
	GL(glDrawElements(GL_TRIANGLES, geom.indexCount * 6, GL_UNSIGNED_INT, (void*)0));
}

void updateGeometry(Geometry& geom)
{
	GL(glBindBuffer(GL_ARRAY_BUFFER, geom.vbo));
	GL(glBufferSubData(GL_ARRAY_BUFFER, 0, geom.vertexCount * 4 * geom.stride, geom.vertices));
}