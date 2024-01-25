#define AG_GAME_DLL
#include "binary_interface.h"
#include <memory>
#include <iostream>


char* mapName;

float screenSpaceMatrix[]
{ 
	2.0f/1920.0f,0.0f,0.0f,-1.0f,
	0.0f,2.0f/1080.0f,0.0f,-1.0f,
	0.0f,0.0f,1.0f,0.0f,
	0.0f,0.0f,0.0f,1.0f
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
	bool modified = false;
};

UVQuad valueToUVMap[]
{
	{ 0.0f, 0.25f, 0.0f, 0.25f },
	{ 0.5f, 1.0f, 0.0f, 0.25f },
	{ 0.0f, 0.5f, 0.25f, 0.5f },
	{ 0.5f, 1.0f, 0.25f, 0.5f },
	{ 0.0f, 0.5f, 0.5f, 0.75f },
	{ 0.5f, 1.0f, 0.5f, 0.75f },
	{ 0.0f, 0.5f, 0.75f, 1.0f }
};

struct Map
{
	bool valid;
	unsigned int xOff, yOff, width, height;
	unsigned int nObjectives;
	unsigned char* data;
};

struct State
{
	unsigned int nLevels = 0;
	unsigned int level = 0;

	bool levelComplete = false;
	bool pauseInput = false;
	unsigned int lastKey = 0;
	double time = 0, timer = 0, period = 0.2;
	unsigned int playerX = 0, playerY = 0;
	unsigned int nObj, completedObj;
	TextureGrid* grid = 0;
	Map* map = 0;
};

Map loadMap(const char* path)
{
	FILE* file = fopen(path,"rb");
	if (file == NULL)
	{
		std::cerr << "Unable to load map file\n";
		return Map{ false };
	}

	Map map{ true };
	if (fscanf(file, "%d,%d,%d,%d", &map.xOff, &map.yOff, &map.width, &map.height) != 4)
	{
		std::cerr << "Invalid map header\n";
		fclose(file);
		return Map{ false };
	}

	map.data = (unsigned char*)malloc(map.width * map.height);
	char c = 0;
	int n = 0;
	while (c >= 0)
	{
		c = fgetc(file);
		if ( c !=32 && (c < 65 || c > 90))
			continue;

		if (n >= map.width * map.height)
			break;

		switch (c)
		{
		case 'P':
			map.data[n] = 1;
			break;
		case 'D':
			map.data[n] = 2;
			++map.nObjectives;
			break;
		case 'C':
			map.data[n] = 3;
			break;
		case 'W':
			map.data[n] = 4;
			break;
		default:
			map.data[n] = 0;
			break;
		}
		++n;
	}

	fclose(file);

	if (n != map.width * map.height)
	{
		std::cerr << "Map dimensions incorrect, continuing..\n";
		map.valid = false;
	}

	return map;
}

void destroyMap(Map& map)
{
	free(map.data);
}

unsigned char getTextureGridValue(TextureGrid& grid, unsigned int x, unsigned int y)
{
	return grid.grid[y * grid.width + x];
}

void setTextureGridValue(TextureGrid& grid, unsigned int x, unsigned int y, unsigned char value)
{
	grid.grid[y * grid.width + x] = value;

	int n = (y * grid.width + x) * 4;
	BTVertex* vertices = (BTVertex*)grid.geom.vertices;
	vertices[n].s = valueToUVMap[value].s1; vertices[n].t = valueToUVMap[value].t1;
	vertices[n + 1].s = valueToUVMap[value].s0; vertices[n + 1].t = valueToUVMap[value].t1;
	vertices[n + 2].s = valueToUVMap[value].s0; vertices[n + 2].t = valueToUVMap[value].t0;
	vertices[n + 3].s = valueToUVMap[value].s1; vertices[n + 3].t = valueToUVMap[value].t0;

	grid.modified = true;
}

void clearTextureGrid(TextureGrid& grid, unsigned char value)
{
	int gridSize = grid.width * grid.height;
	memset(grid.grid, value, gridSize);

	BTVertex* vertices = (BTVertex*)grid.geom.vertices;
	for (int n = 0; n < gridSize*4; ++n)
	{
		vertices[n].s = valueToUVMap[value].s1; vertices[n].t = valueToUVMap[value].t1;
		vertices[n + 1].s = valueToUVMap[value].s0; vertices[n + 1].t = valueToUVMap[value].t1;
		vertices[n + 2].s = valueToUVMap[value].s0; vertices[n + 2].t = valueToUVMap[value].t0;
		vertices[n + 3].s = valueToUVMap[value].s1; vertices[n + 3].t = valueToUVMap[value].t0;
	}

	grid.modified = true; 
}

void initMapNoChecks(Map& map, TextureGrid& grid, State& state)
{
	for (int i = 0; i < map.width; ++i)
	{
		for (int j = 0; j < map.height; ++j)
		{
			unsigned char val = map.data[(map.height - j - 1) * map.width + i];
			if (val == 1)
				state.playerX = i + map.xOff, state.playerY = j + map.yOff;

			setTextureGridValue(grid, i + map.xOff, j + map.yOff, val);
		}
	}

	state.map = &map;
	state.completedObj = 0;
	state.nObj = map.nObjectives;
	return;
}

bool initMap(Map& map, TextureGrid& grid, State& state)
{
	if (map.width > grid.width)
	{
		std::cerr << "Map is too wide!\n";
		return false;
	}
	else if (map.width + map.xOff > grid.width)
	{
		std::cerr << "Map is off screen, adjusting and continuing..\n";
		map.xOff = grid.width - map.width;
	}

	if (map.height > grid.width)
	{
		std::cerr << "Map is too tall!\n";
		return false;
	}
	else if (map.height + map.yOff > grid.height)
	{
		std::cerr << "Map is off screen, adjusting and continuing..\n";
		map.yOff = grid.height - map.height;
	}

	initMapNoChecks(map, grid, state);
	return true;
}

bool initMapCentred(Map& map, TextureGrid& grid, State& state)
{
	if (map.width > grid.width)
	{
		std::cerr << "Map is too wide!\n";
		return false;
	}

	if (map.height > grid.width)
	{
		std::cerr << "Map is too tall!\n";
		return false;
	}

	int ox = map.xOff, oy = map.yOff;
	map.xOff = (grid.width - map.width) / 2;
	map.yOff = (grid.height - map.height) / 2;

	initMapNoChecks(map, grid, state);

	map.xOff = ox, map.yOff = oy;
	return true;
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
			vertices[n].s = valueToUVMap[0].t1; vertices[n].t = valueToUVMap[0].s1;

			vertices[n + 1].x = x + tileWidth - hxG; vertices[n + 1].y = y + hyG;
			vertices[n + 1].s = valueToUVMap[0].t0; vertices[n + 1].t = valueToUVMap[0].s0;

			vertices[n + 2].x = x + tileWidth - hxG; vertices[n + 2].y = y + tileHeight - hyG;
			vertices[n + 2].s = valueToUVMap[0].t0; vertices[n + 2].t = valueToUVMap[0].s0;

			vertices[n + 3].x = x + hxG; vertices[n + 3].y = y + tileHeight - hyG;
			vertices[n + 3].s = valueToUVMap[0].t1; vertices[n + 3].t = valueToUVMap[0].s0;
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
	TextureGrid out { geom, (unsigned char*)calloc(xCells * yCells,sizeof(unsigned char)), xCells, yCells };
	return out;
}

TextBox newCreateTextBox(const char* text, float scale, BitmapFont& font, float r, float g, float b)
{
	TextBox tb{ 0 };
	unsigned int n = tb.length = strlen(text);
	float x = -0.99f, y = 1.0f;
	
	tb.font = &font;
	tb.vertices = new UIVertex[n * 4];

	for (int i = 0; i < n; i++)
	{
		int cv = i * 4;
		UIVertex* p = &tb.vertices[cv];
		float s0 = font.cdata[text[i] - 32].x0 / 4096.0f;
		float s1 = font.cdata[text[i] - 32].x1 / 4096.0f;
		float t0 = font.cdata[text[i] - 32].y0 / 4096.0f;
		float t1 = font.cdata[text[i] - 32].y1 / 4096.0f;

		float xoff = (font.cdata[text[i] - 32].xoff / 1024.0f);
		float x0 = s0 * 4.0f, x1 = s1 * 4.0f;
		float y0 = t0 * 8.0f, y1 = t1 * 8.0f;
		float w = (x1 - x0);
		float h = (y1 - y0);
		float yoff = -(font.cdata[text[i] - 32].yoff / 512.0f + h);

		//Top-Right
		p->x = x + xoff + w; p->y = y + yoff + h;
		p->r = r; p->g = g; p->b = b;
		p->s = s1; p->t = t0;
		p++;

		//Top-Left
		p->x = x + xoff; p->y = y + yoff + h;
		p->r = r; p->g = g; p->b = b;
		p->s = s0; p->t = t0;
		p++;

		//Bottom-Left
		p->x = x + xoff; p->y = y + yoff;
		p->r = r; p->g = g; p->b = b;
		p->s = s0; p->t = t1;
		p++;

		//Bottom-Right
		p->x = x + xoff + w; p->y = y + yoff;
		p->r = r; p->g = g; p->b = b;
		p->s = s1; p->t = t1;

		x += font.cdata[text[i] - 32].xadvance / 1024.0f;
	}

	//Create Vertex Array, Vertex Buffer and Index Buffer
	/*GL(glGenVertexArrays(1, &tb.vao));
	GL(glBindVertexArray(tb.vao));

	GL(glGenBuffers(1, &tb.vbo));
	GL(glBindBuffer(GL_ARRAY_BUFFER, tb.vbo));
	GL(glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertex) * n * 4, tb.vertices, GL_DYNAMIC_DRAW));

	GL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)0));
	GL(glEnableVertexAttribArray(0));
	GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)(sizeof(float) * 2)));
	GL(glEnableVertexAttribArray(1));
	GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)(sizeof(float) * 5)));
	GL(glEnableVertexAttribArray(2));*/

	unsigned int* indices_data = new unsigned int[n * 6];

	int offset = 0;
	for (int i = 0; i < n * 4; i += 4)
	{
		indices_data[offset] = i + 0;
		indices_data[offset + 1] = i + 1;
		indices_data[offset + 2] = i + 2;
		indices_data[offset + 3] = i + 2;
		indices_data[offset + 4] = i + 3;
		indices_data[offset + 5] = i + 0;

		offset += 6;
	}

	/*GL(glGenBuffers(1, &tb.ibo));
	GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tb.ibo));
	GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * n * 6, indices_data, GL_DYNAMIC_DRAW));*/

	delete[] indices_data;
	delete tb.vertices;

	return tb;
}

void translatePlayer(TextureGrid& grid, State& state, unsigned int x, unsigned int y)
{
	unsigned char inFront = getTextureGridValue(grid, state.playerX + x, state.playerY + y);
	if ( inFront == 4 || inFront == 5)
		return;

	if (inFront == 3)
	{
		unsigned int pushTargetX = state.playerX + x + x, pushTargetY = state.playerY + y + y;
		if (pushTargetX < 0 || pushTargetX > grid.width - 1 || pushTargetY < 0 || pushTargetY > grid.height - 1)
			return;

		unsigned char pushTargetValue = getTextureGridValue(grid, pushTargetX, pushTargetY);
		if (pushTargetValue > 2)
			return;
		else if (pushTargetValue == 2)
		{
			setTextureGridValue(grid, pushTargetX, pushTargetY, 5);
			++state.completedObj;
		}
		else
			setTextureGridValue(grid, pushTargetX, pushTargetY, 3);
	}

	if (getTextureGridValue(grid, state.playerX, state.playerY) == 6)
		setTextureGridValue(grid, state.playerX, state.playerY, 2);
	else
		setTextureGridValue(grid, state.playerX, state.playerY, 0);

	state.playerX += x; state.playerY += y;

	if(inFront == 2)
		setTextureGridValue(grid, state.playerX, state.playerY, 6);
	else
		setTextureGridValue(grid, state.playerX, state.playerY, 1);

}

void reloadMap(State& state)
{
	clearTextureGrid(*state.grid, 0);
	initMapCentred(*state.map, *state.grid, state);
}

void updateState(Window& win, State& state)
{
	if (!state.pauseInput)
	{
		if (getKeyDown(win, 65))
		{
			if (state.playerX > 0)
				translatePlayer(*state.grid, state, -1, 0);

			state.pauseInput = true;
			state.lastKey = 65;
			state.timer = getTime();
		}
		if (getKeyDown(win, 68))
		{
			if (state.playerX < state.grid->width - 1)
				translatePlayer(*state.grid, state, 1, 0);

			state.pauseInput = true;
			state.lastKey = 68;
			state.timer = getTime();
		}
		if (getKeyDown(win, 83))
		{
			if (state.playerY > 0)
				translatePlayer(*state.grid, state, 0, -1);

			state.pauseInput = true;
			state.lastKey = 83;
			state.timer = getTime();
		}
		if (getKeyDown(win, 87))
		{
			if (state.playerY < state.grid->height - 1)
				translatePlayer(*state.grid, state, 0, 1);

			state.pauseInput = true;
			state.lastKey = 87;
			state.timer = getTime();
		}
		if (getKeyDown(win, 'R'))
		{
			reloadMap(state);
		}
		if (getKeyDown(win, 264))
		{
			if (state.playerY > 0)
				translatePlayer(*state.grid, state, 0, -1);

			state.pauseInput = true;
			state.lastKey = 264;
			state.timer = getTime();
		}
		if (getKeyDown(win, 265))
		{
			if (state.playerY < state.grid->height - 1)
				translatePlayer(*state.grid, state, 0, 1);

			state.pauseInput = true;
			state.lastKey = 265;
			state.timer = getTime();
		}
		if (getKeyDown(win, 262))
		{
			if (state.playerX < state.grid->width - 1)
				translatePlayer(*state.grid, state, 1, 0);

			state.pauseInput = true;
			state.lastKey = 262;
			state.timer = getTime();
		}
		if (getKeyDown(win, 263))
		{
			if (state.playerX > 0)
				translatePlayer(*state.grid, state, -1, 0);

			state.pauseInput = true;
			state.lastKey = 263;
			state.timer = getTime();
		}
	}
	else
	{
		if (!getKeyDown(win, state.lastKey) || state.timer + state.period < state.time)
		{
			state.pauseInput = false;
		}
	}

	state.time = getTime();

	if (state.nObj == state.completedObj)
	{
		++state.level %= state.nLevels;
		mapName[26] = (char)(state.level+48);

		clearTextureGrid(*state.grid, 0);
		destroyMap(*state.map);
		*state.map = loadMap(mapName);
		initMapCentred(*state.map, *state.grid, state);
	}
	if (getKeyDown(win, 256))
	{
		win.shouldClose = true;
	}
}

void entryPoint(Window& win)
{

	Shader basic = createShader("src/games/sokoban/shaders/basic.vs", "src/games/sokoban/shaders/basic.fs");
	Shader sokobanShader = createShader("src/games/sokoban/shaders/main.vs", "src/games/sokoban/shaders/main.fs");
	Shader textShader = createShader("src/shaders/text.vs", "src/shaders/text.fs");

	BitmapFont font = loadFontBitmap("D:/AG/assets/Spectral.ttf");

	TextBox titleTextBox = createTextBox("Sokoban", 1.0f, font, 1.0f, 1.0f, 1.0f);

	float color[] = {1.0f,1.0f,1.0f,0.8f,0.8f,0.8f};
	BasicGrid floorGrid = createBasicGrid(1080.0f, 1080.0f, 15, 15, 0.0f, 0.0f, color, 2);
	TextureGrid gameGrid = createTextureGrid(1080.0f, 1080.0f,15,15,0.0f,0.0f);

	Image im = loadImage("assets/test.png");
	unsigned int tex = createTexture(im);

	mapName = (char*)malloc(32);
	memcpy(mapName, "src/games/sokoban/maps/map0.map", 32);
	Map map = loadMap(mapName);

	State state;
	state.grid = &gameGrid;
	state.nLevels = 5;

	initMapCentred(map, gameGrid, state);

	useShader(basic);
	bindMat4(basic.ID, "mat", screenSpaceMatrix);
	useShader(sokobanShader);
	bindMat4(sokobanShader.ID, "mat", screenSpaceMatrix);
	float adjust[]{ (float)(1920 - 1080) / 1920.0f * 2.0f, 0.0f };
	bindVec2(sokobanShader.ID, "trans", adjust);
	while (!win.shouldClose)
	{
		updateTextureGrid(gameGrid);

		clearFrameBuffer();
		useShader(basic);
		
		drawBasicGrid(floorGrid,(float)(1920-1080)/1920.0f*2.0f ,0.001f,0.0f);
		useShader(sokobanShader);
		drawGeometry(gameGrid.geom);

		updateState(win, state);
		updateWindow(win);
	}
}