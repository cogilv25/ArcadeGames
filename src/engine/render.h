#ifndef AG_RENDER_H
#define AG_RENDER_H

struct Image
{
    int width, height;
    unsigned char* data;
};

struct Geometry
{
    unsigned int vao;
    unsigned int vbo;
    unsigned int ibo;

    float* vertices;
    unsigned int* indices;

    unsigned int vertexCount;
    unsigned int indexCount;
    unsigned int stride;
};

struct BasicGrid
{
    float width;
    float height;
    unsigned int xCells;
    unsigned int yCells;
    float xGap;
    float yGap;
    float* colors;
    unsigned int colorCount;

    Geometry geom;
};

struct BasicVertex
{
    float x, y;
    float r, g, b;
};

struct Shader
{
    unsigned int ID;
    bool valid;
};
struct Rect
{
    unsigned int vao;
    unsigned int vbo;
    unsigned int ibo;

    float vertices[4 * 5];
    unsigned int indices[6];
};

#ifndef AG_GAME_DLL

Shader createShader(const char* vsCode, const char* fsCode);

void useShader(const Shader&);
void bindMat4(unsigned int shaderID, const char* name, float* matrix);
void bindVec2(unsigned int shaderID, const char* name, float* vector);
void bindFloat(unsigned int shaderID, const char* name, float f);
void bindTexture(unsigned int id);

void clearFrameBuffer();

Image loadImage(const char* path);
void destroyImage(Image&);

unsigned int createTexture(Image&);
void destroyTexture(unsigned int ID);

Geometry createGeometry(float* vertices, unsigned int* indices, unsigned int* compPerLoc, unsigned int vertexCount, unsigned int indexCount, unsigned int locCount, unsigned int stride);
void drawGeometry(Geometry&);
void updateGeometry(Geometry&);

Rect createRect(float width, float height, float r, float g, float b);
void drawRect(Rect&, float x, float y);

BasicGrid createBasicGrid(float width, float height, unsigned int xCellCount, unsigned int yCellCount, float xCellSpacing, float yCellSpacing, float * colors, unsigned int colorCount);
void drawBasicGrid(BasicGrid&, float x, float y, float z);

#endif

#endif