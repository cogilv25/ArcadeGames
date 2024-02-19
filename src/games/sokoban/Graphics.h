#pragma once

struct TexturedQuad
{
	float x, y;
	Geometry geom;
};

struct UVQuad
{
	float s0, s1, t0, t1;
};

struct BTVertex
{
	float x, y, s, t;
};

struct TextureGrid
{
	Geometry geom;
	unsigned char* grid;
	unsigned int width;
	unsigned int height;
	float* xPoints, * yPoints;
	bool modified = false;
};

TexturedQuad createTexturedQuad(float width, float height, float s0, float s1, float t0, float t1)
{
	TexturedQuad quad{ 0 };
	BTVertex* vertices = (BTVertex*)malloc(sizeof(BTVertex) * 4);
	unsigned int* indices = (unsigned int*)malloc(sizeof(unsigned int) * 6);
	BTVertex* p = (BTVertex*)vertices;

	//Top-Right
	p->x = width; p->y = height;
	p->s = s0; p->t = t1;
	p++;

	//Top-Left
	p->x = 0.0f; p->y = height;
	p->s = s1; p->t = t1;
	p++;

	//Bottom-Left
	p->x = 0.0f; p->y = 0.0f;
	p->s = s1; p->t = t0;
	p++;

	//Bottom-Right
	p->x = width; p->y = 0.0f;
	p->s = s0; p->t = t0;

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 0;

	unsigned int cPL[]{ 2,2 };
	quad.geom = createGeometry((float*)vertices, indices, cPL, 4, 6, 2, 4);

	return quad;
}

void destroyTexturedQuad(TexturedQuad& quad)
{
	free(quad.geom.vertices);
	free(quad.geom.indices);
}