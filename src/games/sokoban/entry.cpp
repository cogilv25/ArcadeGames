#define AG_GAME_DLL
#include "binary_interface.h"
#include <memory>

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
	bool modified = false;
};

UVQuad valueToUVMap[]
{
	{ 0.0f, 0.5f, 0.0f, 0.5f },
	{ 0.5f, 1.0f, 0.0f, 0.5f },
	{ 0.0f, 0.5f, 0.5f, 1.0f },
	{ 0.5f, 1.0f, 0.5f, 1.0f }
};

void setTextureGridValue(TextureGrid& grid, unsigned int x, unsigned int y, unsigned char value)
{
	grid.grid[y * grid.width + x] = value;

	int n = (y * grid.width + x) * 4;
	BTVertex* vertices = (BTVertex*)grid.geom.vertices;
	vertices[n].s = valueToUVMap[value].s0; vertices[n].t = valueToUVMap[value].t1;
	vertices[n + 1].s = valueToUVMap[value].s0; vertices[n + 1].t = valueToUVMap[value].t0;
	vertices[n + 2].s = valueToUVMap[value].s1; vertices[n + 2].t = valueToUVMap[value].t0;
	vertices[n + 3].s = valueToUVMap[value].s1; vertices[n + 3].t = valueToUVMap[value].t1;

	grid.modified = true;
}

void updateTextureGrid(TextureGrid& grid)
{
	if (!grid.modified)
		return;

	updateGeometry(grid.geom);
	grid.modified = false;
}

TextureGrid createTextureGrid(float width, float height, unsigned int xCells, unsigned int yCells, float xGap, float yGap)
{
	BTVertex* vertices = (BTVertex*)malloc(xCells * yCells * 4 * sizeof(BTVertex));

	float tileWidth = (width - xGap) / (float)xCells;
	float tileHeight = (height - yGap) / (float)yCells;
	float hxG = xGap / 2.0f, hyG = yGap / 2.0f;
	for (int i = 0; i < xCells; i++)
	{
		for (int j = 0; j < yCells; j++)
		{

			float x = hxG + (i * tileWidth - 1.0f);
			float y = hyG + (j * tileHeight - 1.0f);
			int m = j * xCells + i;
			int n = m * 4;

			vertices[n].x = x + hxG; vertices[n].y = y + hyG;
			vertices[n].s = valueToUVMap[0].s0; vertices[n].t = valueToUVMap[0].t1;

			vertices[n + 1].x = x + tileWidth - hxG; vertices[n + 1].y = y + hyG;
			vertices[n + 1].s = valueToUVMap[0].s0; vertices[n + 1].t = valueToUVMap[0].t0;

			vertices[n + 2].x = x + tileWidth - hxG; vertices[n + 2].y = y + tileHeight - hyG;
			vertices[n + 2].s = valueToUVMap[0].s1; vertices[n + 2].t = valueToUVMap[0].t0;

			vertices[n + 3].x = x + hxG; vertices[n + 3].y = y + tileHeight - hyG;
			vertices[n + 3].s = valueToUVMap[0].s1; vertices[n + 3].t = valueToUVMap[0].t1;
		}
	}

	unsigned int* indices_data = (unsigned int*)malloc(xCells * yCells * 6 * sizeof(unsigned int));

	int offset = 0;
	for (int i = 0; i < xCells * yCells * 4; i += 4)
	{
		indices_data[offset] = i + 0;
		indices_data[offset + 1] = i + 1;
		indices_data[offset + 2] = i + 2;
		indices_data[offset + 3] = i + 2;
		indices_data[offset + 4] = i + 3;
		indices_data[offset + 5] = i + 0;

		offset += 6;
	}

	unsigned int cPL[]{ 2,2 };
	Geometry geom = createGeometry((float*)vertices, indices_data, cPL, xCells * yCells * 4, xCells * yCells * 6, 2, 4);
	TextureGrid out { geom, (unsigned char*)malloc(sizeof(unsigned char) * xCells * yCells), xCells, yCells };
	return out;
}

void entryPoint(Window& win)
{

	Shader basic = createShader("src/shaders/basic.vs", "src/shaders/basic.fs");
	Shader sokobanShader = createShader("src/games/sokoban/shaders/main.vs", "src/games/sokoban/shaders/main.fs");
	
	float color[] = {1.0f,1.0f,1.0f,0.8f,0.8f,0.8f};
	BasicGrid floorGrid = createBasicGrid(2.0f, 2.0f, 31, 17, 0.0f, 0.0f, color, 2);
	TextureGrid gameGrid = createTextureGrid(2.0f,2.0f,31,17,0.0f,0.0f);

	Image im = loadImage("assets/test.png");
	unsigned int tex = createTexture(im);

	unsigned int playerX = 0, playerY = 0;
	setTextureGridValue(gameGrid, playerX, playerY, 1);


	bool pauseInput = false;
	unsigned int lastKey = 0;
	double time, timer = 0, period = 0.2;
	while (!win.shouldClose)
	{
		updateTextureGrid(gameGrid);

		clearFrameBuffer();
		useShader(basic);
		drawBasicGrid(floorGrid,0.0f,0.0f,0.9f);
		useShader(sokobanShader);
		drawGeometry(gameGrid.geom);

		updateWindow(win);

		if (getKeyDown(win, 65) && !pauseInput)
		{
			if (playerX > 0)
			{
				setTextureGridValue(gameGrid, playerX, playerY, 0);
				playerX--;
				setTextureGridValue(gameGrid, playerX, playerY, 1);
			}
			pauseInput = true;
			lastKey = 65;
			timer = getTime();
		}
		if (getKeyDown(win, 68) && !pauseInput)
		{
			if (playerX < gameGrid.width - 1)
			{
				setTextureGridValue(gameGrid, playerX, playerY, 0);
				playerX++;
				setTextureGridValue(gameGrid, playerX, playerY, 1);
			}
			pauseInput = true;
			lastKey = 68;
			timer = getTime();
		}
		if (getKeyDown(win, 83) && !pauseInput)
		{
			if (playerY > 0)
			{
				setTextureGridValue(gameGrid, playerX, playerY, 0);
				playerY--;
				setTextureGridValue(gameGrid, playerX, playerY, 1);
			}
			pauseInput = true;
			lastKey = 83;
			timer = getTime();
		}
		if (getKeyDown(win, 87) && !pauseInput)
		{
			if (playerY < gameGrid.height - 1)
			{
				setTextureGridValue(gameGrid, playerX, playerY, 0);
				playerY++;
				setTextureGridValue(gameGrid, playerX, playerY, 1);
			}
			pauseInput = true;
			lastKey = 87;
			timer = getTime();
		}


		time = getTime();
		if (pauseInput)
		{
			if (!getKeyDown(win, lastKey) || timer + period < time)
			{
				pauseInput = false;
			}
		}

		if (getKeyDown(win, 256))
		{
			win.shouldClose = true;
		}
	}
}