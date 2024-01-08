#ifndef AG_RENDER_H
#define AG_RENDER_H

struct Shader;
struct Geometry;

Shader createShader(const char* vsCode, const char* fsCode);

void useShader(Shader&);

void clearFrameBuffer();

Geometry createGeometry(float* vertices, unsigned int* indices, unsigned int* compPerLoc, unsigned int vertexCount, unsigned int indexCount, unsigned int locCount, unsigned int stride);
void drawGeometry(Geometry&);
void updateGeometry(Geometry&);

#endif