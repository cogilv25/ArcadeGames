#ifndef AG_RENDER_H
#define AG_RENDER_H

struct Shader;
struct Geometry;
struct Rect;
struct BasicGrid;
struct Image;

Shader createShader(const char* vsCode, const char* fsCode);

void useShader(Shader&);
void bindMat4(unsigned int shaderID, const char* name, float* matrix);
void bindVec2(unsigned int shaderID, const char* name, float* vector);

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