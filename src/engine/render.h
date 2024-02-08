#ifndef AG_RENDER_H
#define AG_RENDER_H

struct Image
{
    int width, height;
    unsigned char* data;
};

struct StaticGeometry
{
    unsigned int vao;
    unsigned int indexCount;
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


#ifndef AG_GAME_DLL

Shader createShader(const char* vsCode, const char* fsCode);
void destroyShader(Shader&);

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

StaticGeometry createStaticGeometry(float* vertices, unsigned int* indices, unsigned int* compPerLoc, unsigned int vertexCount, unsigned int indexCount, unsigned int locCount, unsigned int stride);
void drawStaticGeometry(StaticGeometry&);

Geometry createGeometry(float* vertices, unsigned int* indices, unsigned int* compPerLoc, unsigned int vertexCount, unsigned int indexCount, unsigned int locCount, unsigned int stride);
void drawGeometry(Geometry&);
void updateGeometry(Geometry&);
void destroyGeometry(Geometry&);

BasicGrid createBasicGrid(float width, float height, unsigned int xCellCount, unsigned int yCellCount, float xCellSpacing, float yCellSpacing, float * colors, unsigned int colorCount);
void drawBasicGrid(BasicGrid&, float x, float y, float z);
void destroyBasicGrid(BasicGrid& grid);

#endif

#endif