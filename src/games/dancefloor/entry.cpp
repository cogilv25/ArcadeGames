#define AG_GAME_DLL
#include "binary_interface.h"
#include <iostream>

struct DanceFloor
{
	unsigned int width;
	unsigned int height;

	double period;
	double lastUpdate;

	Geometry geom;
};

float palette[][3]
{
	{0.8f,0.8f,0.8f},
	{0.8f,0.0f,0.0f},
	{0.0f,0.0f,0.8f},
	{0.0f,0.8f,0.0f},
	{0.8f,0.8f,0.0f},
	{0.8f,0.0f,0.8f}
};

DanceFloor createDanceFloor(unsigned int width, unsigned int height, double period)
{
	DanceFloor df{ width, height, period, 0 };


	BasicVertex* vertices  = (BasicVertex*)malloc(width * height * 4 * sizeof(BasicVertex));

	//Ekk
	float tileWidth = 2.0f / (float)width;
	float tileHeight = 2.0f / (float)height;
	srand(clock());
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{

			float x = i * tileWidth - 1.0f;
			float y = j * tileHeight - 1.0f - (2.0f / 9.0f);
			int n = (j * width + i) * 4;
			unsigned int c = rand() % 6;

			vertices[n].x = x + 0.0032f; vertices[n].y = y + 0.0032f;
			vertices[n].r = palette[c][0]; vertices[n].g = palette[c][1]; vertices[n].b = palette[c][2];

			vertices[n+1].x = x + tileWidth - 0.0032f; vertices[n+1].y = y + 0.0032f;
			vertices[n + 1].r = palette[c][0]; vertices[n+1].g = palette[c][1]; vertices[n+1].b = palette[c][2];

			vertices[n+2].x = x + tileWidth - 0.0032f; vertices[n+2].y = y + tileHeight - 0.0032f;
			vertices[n+2].r = palette[c][0]; vertices[n+2].g = palette[c][1]; vertices[n+2].b = palette[c][2];

			vertices[n+3].x = x + 0.0032f; vertices[n+3].y = y + tileHeight - 0.0032f;
			vertices[n+3].r = palette[c][0]; vertices[n+3].g = palette[c][1]; vertices[n+3].b = palette[c][2];
		}
	}

	unsigned int* indices_data = (unsigned int*)malloc(width * height * 6 * sizeof(unsigned int));

	int offset = 0;
	for (int i = 0; i < width * height * 4; i += 4)
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
	df.geom = createGeometry((float*)vertices, indices_data, attribSizes, width * height * 4, width * height * 6, 2, 5);

	return df;
}

void destroyDanceFloor(DanceFloor& df)
{
	free(df.geom.vertices);
	free(df.geom.indices);
	destroyGeometry(df.geom);
}

void updateDanceFloor(DanceFloor& df)
{
	if (df.lastUpdate + df.period > getTime())
		return;

	for (int i = 0; i < df.width; i++)
	{
		for (int j = 0; j < df.height; j++)
		{
			float * color = palette[rand()%6];
			int n = (j * df.width + i) * 4;
			BasicVertex* vertices = ((BasicVertex*)df.geom.vertices) + n;

			vertices[0].r = color[0]; vertices[0].g = color[1]; vertices[0].b = color[2];
			vertices[1].r = color[0]; vertices[1].g = color[1]; vertices[1].b = color[2];
			vertices[2].r = color[0]; vertices[2].g = color[1]; vertices[2].b = color[2];
			vertices[3].r = color[0]; vertices[3].g = color[1]; vertices[3].b = color[2];
		}
	}

	updateGeometry(df.geom);

	df.lastUpdate = getTime();
}


void dllMain(Window& win)
{
	Shader basicShader = createShader("src/shaders/basic.vs", "src/shaders/basic.fs");
	const Shader& textShader = getEngineShader(TEXT);

	Font font = loadFont("assets/Spectral.ttf");
	if (!font.valid)
		std::cerr << "Could not load font @ assets/Spectral.ttf\n";

	StaticText tb = createStaticText("This is the first thing I made, and just a simple little example...", font);

	DanceFloor floor = createDanceFloor(16, 9, 0.3);
	while (!win.shouldClose)
	{
		useShader(basicShader);
		drawGeometry(floor.geom);

		useShader(textShader);
		drawStaticText(tb, -0.99f, 0.85f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);

		updateDanceFloor(floor);

		updateWindow(win);
		if (getKeyDown(win, 256))
		{
			win.shouldClose = true;
		}
		clearFrameBuffer();
	}

	destroyDanceFloor(floor);
	destroyStaticText(tb);
	destroyFont(font);
	destroyShader(basicShader);
}