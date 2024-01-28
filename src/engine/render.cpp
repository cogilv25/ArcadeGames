#include "render.h"
#include "glad/glad.h"
#include "utilities.h"
#include "binary_interface.h"
#include <memory>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

extern std::shared_ptr<char> loadFileAsCharArray(const char* path);
unsigned int currentShader = 0;


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
	currentShader = s.ID;
	GL(glUseProgram(currentShader));
}

void clearFrameBuffer()
{
	GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

Image loadImage(const char* path)
{
	int w, h, n;
	unsigned char *data = stbi_load(path, &w, &h, &n, 4);
	return Image {w,h,data};
}

void destroyImage(Image& image)
{
	stbi_image_free(image.data);
}

unsigned int createTexture(Image& image)
{
	unsigned int id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
	glGenerateMipmap(GL_TEXTURE_2D);
	return id;
}

void bindTexture(unsigned int id)
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void destroyTexture(unsigned int ID)
{
	glDeleteTextures(1, &ID);
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
	GL(glBufferSubData(GL_ARRAY_BUFFER, 0, geom.vertexCount * geom.stride * sizeof(float), geom.vertices));
}

Rect createRect(float w, float h, float r, float g, float b)
{
	Rect rect{ 0, 0, 0 };
	BasicVertex* p = (BasicVertex*)rect.vertices;

	//Top-Right
	p->x = w; p->y = h;
	p->r = r; p->g = g; p->b = b;
	p++;

	//Top-Left
	p->x = 0.0f; p->y = h;
	p->r = r; p->g = g; p->b = b;
	p++;

	//Bottom-Left
	p->x = 0.0f; p->y = 0.0f;
	p->r = r; p->g = g; p->b = b;
	p++;

	//Bottom-Right
	p->x = w; p->y = 0.0f;
	p->r = r; p->g = g; p->b = b;

	rect.indices[0] = 0;
	rect.indices[1] = 1;
	rect.indices[2] = 2;
	rect.indices[3] = 2;
	rect.indices[4] = 3;
	rect.indices[5] = 0;


	//Create Vertex Array, Vertex Buffer and Index Buffer
	GL(glGenVertexArrays(1, &rect.vao));
	GL(glBindVertexArray(rect.vao));

	GL(glGenBuffers(1, &rect.vbo));
	GL(glBindBuffer(GL_ARRAY_BUFFER, rect.vbo));
	GL(glBufferData(GL_ARRAY_BUFFER, sizeof(BasicVertex) * 4, rect.vertices, GL_DYNAMIC_DRAW));

	
	GL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*)0));
	GL(glEnableVertexAttribArray(0));
	GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*)(sizeof(float)*2)));
	GL(glEnableVertexAttribArray(1));

	GL(glGenBuffers(1, &rect.ibo));
	GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect.ibo));
	GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, rect.indices, GL_DYNAMIC_DRAW));

	return rect;
}

void drawRect(Rect& r, float x, float y)
{
	GL(glBindVertexArray(r.vao));
	GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0));
}

BasicGrid createBasicGrid(float w, float h, unsigned int xC, unsigned int yC, float xS, float yS, float* c, unsigned int cC)
{
	BasicGrid grid{ w,h,xC,yC,xS,yS,c,cC };

	BasicVertex* vertices = new BasicVertex[xC * yC * 4];

	//Ekk
	float tileWidth = (w-xS) / (float)xC;
	float tileHeight = (h-yS) / (float)yC;
	float hxG = xS / 2.0f, hyG = yS / 2.0f;
	for (int i = 0; i < xC; i++)
	{
		for (int j = 0; j < yC; j++)
		{

			float x = hxG + (i * tileWidth - 1.0f);
			float y = hyG + (j * tileHeight - 1.0f);
			int m = j * xC + i;
			int n = m * 4;

			vertices[n].x = x + hxG; vertices[n].y = y + hyG;
			vertices[n].r = c[(m%cC)*3]; vertices[n].g = c[(m%cC)*3+1]; vertices[n].b = c[(m%cC)*3+2];

			vertices[n + 1].x = x + tileWidth - hxG; vertices[n + 1].y = y + hyG;
			vertices[n+1].r = c[(m%cC)*3]; vertices[n+1].g = c[(m%cC)*3+1]; vertices[n+1].b = c[(m%cC)*3+2];

			vertices[n + 2].x = x + tileWidth - hxG; vertices[n + 2].y = y + tileHeight - hyG;
			vertices[n+2].r = c[(m%cC)*3]; vertices[n+2].g = c[(m%cC)*3+1]; vertices[n+2].b = c[(m%cC)*3+2];

			vertices[n + 3].x = x + hxG; vertices[n + 3].y = y + tileHeight - hyG;
			vertices[n+3].r = c[(m%cC)*3]; vertices[n+3].g = c[(m%cC)*3+1]; vertices[n+3].b = c[(m%cC)*3+2];
		}
	}

	unsigned int* indices_data = (unsigned int*)malloc(xC * yC * 6 * sizeof(unsigned int));

	int offset = 0;
	for (int i = 0; i < xC * yC * 4; i += 4)
	{
		indices_data[offset] = i + 0;
		indices_data[offset + 1] = i + 1;
		indices_data[offset + 2] = i + 2;
		indices_data[offset + 3] = i + 2;
		indices_data[offset + 4] = i + 3;
		indices_data[offset + 5] = i + 0;

		offset += 6;
	}

	unsigned int attribSizes[] = { 2,3 };
	grid.geom = createGeometry((float*)vertices, indices_data, attribSizes, xC * yC * 4, xC * yC * 6, 2, 5);

	return grid;
}

void drawBasicGrid(BasicGrid& grid, float x, float y, float z)
{
	GL(unsigned int loc = glGetUniformLocation(currentShader, "trans"));
	GL(glUniform3f(loc, x, y, z));

	drawGeometry(grid.geom);
}

void bindMat4(unsigned int shaderID, const char* name, float* mat)
{
	unsigned int loc = GL(glGetUniformLocation(shaderID, name));
	GL(glUniformMatrix4fv(loc, 1, false, mat);)
}
 
void bindVec2(unsigned int shaderID, const char* name, float* vec)
{
	unsigned int loc = GL(glGetUniformLocation(shaderID, name));
	GL(glUniform2f(loc, vec[0], vec[1]));
}

void bindFloat(unsigned int shaderID, const char* name, float f)
{
	unsigned int loc = GL(glGetUniformLocation(shaderID, name));
	GL(glUniform1f(loc, f));
}
