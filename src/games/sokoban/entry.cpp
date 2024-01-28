#define AG_GAME_DLL
#include "binary_interface.h"
#include <memory>
#include <iostream>

#define LEVELS 5

char* mapName;
char* levelText;
char* timerText;
char* stepsText;
TextBox levelTextBox;
TextBox timerTextBox;
TextBox stepsTextBox;
unsigned int steps;
double timerStartTime;
double timerDisplayTimer;
double levelTime;

bool highscoresVisible = false;
double hsTimes[LEVELS] { 1.843, 5.820, 4.025, 33.019, 40.428 };
unsigned int hsSteps[LEVELS] { 15, 31, 23, 151, 175 };
unsigned int highscoresAchieved = 0;
TextBox hsTimesTextBoxes[LEVELS];
TextBox hsStepsTextBoxes[LEVELS];
TextBox hsLevelsTextBoxes[LEVELS];
char* hsLevelsText[LEVELS] { "1", "2", "3", "4", "5" };
char* hsTimesText[LEVELS] { (char*)malloc(9), (char*)malloc(9), (char*)malloc(9), (char*)malloc(9), (char*)malloc(9)};
char* hsStepsText[LEVELS] { (char*)malloc(4), (char*)malloc(4), (char*)malloc(4), (char*)malloc(4), (char*)malloc(4) };
float hsTableXPositions[] { 0.04f, 0.356f, 0.64f };
float hsTableYPositions[]{ 1.45f, 1.57f, 1.69f, 1.81f, 1.93f };

float hsTimesColours[LEVELS * 3];
float hsStepsColours[LEVELS * 3];

float completedColour[]{ 219, 188, 76 };

float screenSpaceMatrix[]
{ 
	2.0f/1920.0f,0.0f,0.0f,-1.0f,
	0.0f,2.0f/1080.0f,0.0f,-1.0f,
	0.0f,0.0f,1.0f,0.0f,
	0.0f,0.0f,0.0f,1.0f
};

struct TexturedQuad
{
	float x, y;
	Geometry geom;
};


TexturedQuad menu1BG;
TexturedQuad menu2BG;

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

	bool levelStarted = false;
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
	if (fscanf(file, "%d,%d", &map.width, &map.height) != 2)
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
	state.levelStarted = false;
	levelTime = 0;
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

			float x = (i * tileWidth - 1.0f);
			float y = (j * tileHeight - 1.0f);
			int m = j * xCells + i;
			int n = m * 4;

			vertices[n].x = x; vertices[n].y = y;
			vertices[n].s = valueToUVMap[0].t1; vertices[n].t = valueToUVMap[0].s1;

			vertices[n + 1].x = x + tileWidth; vertices[n + 1].y = y;
			vertices[n + 1].s = valueToUVMap[0].t0; vertices[n + 1].t = valueToUVMap[0].s0;

			vertices[n + 2].x = x + tileWidth; vertices[n + 2].y = y + tileHeight;
			vertices[n + 2].s = valueToUVMap[0].t0; vertices[n + 2].t = valueToUVMap[0].s0;

			vertices[n + 3].x = x; vertices[n + 3].y = y + tileHeight;
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

TextBox newCreateTextBox(Font& font, unsigned int maxLength)
{
	TextBox tb{ 0 };
	unsigned int n = tb.length = maxLength;
	
	tb.font = &font;
	tb.vertices = (UIVertex*) malloc(n * 4 * sizeof(UIVertex));

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


	unsigned int cPL[]{ 2,3,2 };
	tb.geom = createGeometry((float*)tb.vertices, indices_data, cPL, tb.length * 4, tb.length * 6, 3, 7);

	return tb;
}

void populateTextBox(TextBox& tb, const char* text, float scale, float r, float g, float b)
{
	Font& font = *tb.font;
	float x = -1.0f, y = 1.0f;
	bool eos = false;
	for (int i = 0; i < tb.length; i++)
	{
		if (text[i] <= 0 || text[i] > 126 )
			eos = true;
		int cv = i * 4;
		char c = (eos ? ' ' : text[i]) - 32;
		UIVertex* p = &tb.vertices[cv];
		float s0 = font.cdata[c].x0 / 4096.0f;
		float s1 = font.cdata[c].x1 / 4096.0f;
		float t0 = font.cdata[c].y0 / 4096.0f;
		float t1 = font.cdata[c].y1 / 4096.0f;

		float xoff = (font.cdata[c].xoff / 1024.0f);
		float x0 = s0 * 4.0f, x1 = s1 * 4.0f;
		float y0 = t0 * 8.0f, y1 = t1 * 8.0f;
		float w = (x1 - x0);
		float h = (y1 - y0);
		float yoff = -(font.cdata[c].yoff / 512.0f + h);

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

		x += font.cdata[c].xadvance / 1024.0f;
	}

	updateGeometry(tb.geom);
}

void updateHighScore(unsigned int level, double time, unsigned int steps)
{
	bool newTime = hsTimes[level] > time;
	bool newSteps = hsSteps[level] > steps;
	bool sameSteps = hsSteps[level] == steps;

	float r[2]{ 219.0f / 255.0f, 0.0f };
	float g[2]{ 188.0f / 255.0f, 135.0f/255.0f };
	float b[2]{ 35.0f / 255.0f, 1.0f };

	if (newTime)
	{
		hsTimes[level] = time;
		sprintf(hsTimesText[level], "%.3fs", time);
		if (newSteps || sameSteps)
		{
			hsSteps[level] = steps;
			sprintf(hsStepsText[level], "%d", steps);
			populateTextBox(hsStepsTextBoxes[level], hsStepsText[level], 1.0f, r[1], g[1], b[1]);
			populateTextBox(hsTimesTextBoxes[level], hsTimesText[level], 1.0f, r[1], g[1], b[1]);
		}
		else
		{
			populateTextBox(hsTimesTextBoxes[level], hsTimesText[level], 1.0f, r[0], g[0], b[0]);
		}
	}
	else if (newSteps)
	{
		hsSteps[level] = steps;
		sprintf(hsStepsText[level], "%d", steps);
		populateTextBox(hsStepsTextBoxes[level], hsStepsText[level], 1.0f, r[0], g[0], b[0]);
	}

}

void translatePlayer(TextureGrid& grid, State& state, unsigned int x, unsigned int y)
{
	unsigned char inFront = getTextureGridValue(grid, state.playerX + x, state.playerY + y);
	if (inFront == 4 || inFront == 5)
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
	if (!state.levelStarted)
	{
		state.levelStarted = true;
		timerStartTime = getTime();
	}
	sprintf(stepsText, "Steps: %d", steps < 1000 ? ++steps : 1000 );
	populateTextBox(stepsTextBox, stepsText, 1.0f, 1.0f, 1.0f, 1.0f);

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
			steps = 0;
			sprintf(stepsText, "Steps: %d", steps);
			populateTextBox(stepsTextBox, stepsText, 1.0f, 1.0f, 1.0f, 1.0f);
			timerStartTime = getTime();
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

		bool resetLevel = false;
		int newLevel = 0;

		

		for (int i = LEVELS-1; i >= 0; --i)
			if ((newLevel = i * (resetLevel = getKeyDown(win, 290 + i))) > 0)
				break;

		if (resetLevel)
		{
			state.level = newLevel;
			state.pauseInput = true;
			mapName[26] = (char)(state.level + 48);
			levelText[7] = mapName[26] + 1;
			populateTextBox(levelTextBox, levelText, 1.0f, 1.0f, 1.0f, 1.0f);

			clearTextureGrid(*state.grid, 0);
			destroyMap(*state.map);
			*state.map = loadMap(mapName);
			initMapCentred(*state.map, *state.grid, state);

			sprintf(stepsText, "Steps: %d", steps = 0);
			populateTextBox(stepsTextBox, stepsText, 1.0f, 1.0f, 1.0f, 1.0f);
			timerStartTime = getTime();
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
	timerDisplayTimer = state.time;
	if (state.levelStarted)
	{
		levelTime = state.time - timerStartTime;
		levelTime = levelTime > 600.0 ? 600.0 : levelTime;
	}

	sprintf(timerText, "Time: %.3fs", levelTime);
	populateTextBox(timerTextBox, timerText, 1.0f, 1.0f, 1.0f, 1.0f);

	if (state.nObj == state.completedObj)
	{
		updateHighScore(state.level, levelTime, steps);

		++state.level %= state.nLevels;
		mapName[26] = (char)(state.level+48);
		levelText[7] = mapName[26] + 1;
		populateTextBox(levelTextBox, levelText, 1.0f, 1.0f, 1.0f, 1.0f);

		clearTextureGrid(*state.grid, 0);
		destroyMap(*state.map);
		*state.map = loadMap(mapName);
		initMapCentred(*state.map, *state.grid, state);

		sprintf(stepsText, "Steps: %d", steps = 0);
		populateTextBox(stepsTextBox, stepsText, 1.0f, 1.0f, 1.0f, 1.0f);
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

	Font font = loadFont("D:/AG/assets/Spectral.ttf");

	TextBox titleTextBox = createTextBox("Sokoban", 1.0f, font, 1.0f, 1.0f, 1.0f);
	TextBox highscoresTextBox = createTextBox("Highscores", 1.0f, font, 1.0f, 1.0f, 1.0f);
	TextBox hsLevelTextBox = createTextBox("Level", 1.0f, font, 1.0f, 1.0f, 1.0f);
	TextBox hsStepsTextBox = createTextBox("Steps", 1.0f, font, 1.0f, 1.0f, 1.0f);
	TextBox hsTimeTextBox = createTextBox("Time", 1.0f, font, 1.0f, 1.0f, 1.0f);

	levelTextBox = newCreateTextBox(font, 9);
	populateTextBox(levelTextBox, "Level: 1", 1.0f, 1.0f, 1.0f, 1.0f);

	timerTextBox = newCreateTextBox(font, 15);
	populateTextBox(timerTextBox, "Time: 0.000s", 1.0f, 1.0f, 1.0f, 1.0f);

	stepsTextBox = newCreateTextBox(font, 12);
	populateTextBox(stepsTextBox, "Steps: 0", 1.0f, 1.0f, 1.0f, 1.0f);

	for (int i = 0; i < LEVELS; ++i)
	{
		hsLevelsTextBoxes[i] = newCreateTextBox(font, 2);
		populateTextBox(hsLevelsTextBoxes[i], hsLevelsText[i], 1.0f, 1.0f, 1.0f, 1.0f);

		hsStepsTextBoxes[i] = newCreateTextBox(font, 4);
		sprintf(hsStepsText[i], "%d", hsSteps[i]);
		populateTextBox(hsStepsTextBoxes[i], hsStepsText[i], 1.0f, 1.0f, 1.0f, 1.0f);

		hsTimesTextBoxes[i] = newCreateTextBox(font, 9);
		sprintf(hsTimesText[i], "%.3fs", hsTimes[i]);
		populateTextBox(hsTimesTextBoxes[i], hsTimesText[i], 1.0f, 1.0f, 1.0f, 1.0f);

		for (int j = 0; j < 3; j++)
		{
			hsTimesColours[i * 3 + j] = 1.0f;
			hsStepsColours[i * 3 + j] = 1.0f;
		}

	}


	float color[] = {1.0f,1.0f,1.0f,0.8f,0.8f,0.8f};
	BasicGrid floorGrid = createBasicGrid(1944.0f, 1080.0f, 27, 15, 0.0f, 0.0f, color, 2);
	TextureGrid gameGrid = createTextureGrid(1080.0f, 1080.0f,15,15,0.0f,0.0f);

	Image im = loadImage("assets/test.png");
	unsigned int tex = createTexture(im);

	Image menuBGImg = loadImage("assets/menubgtop.png");
	unsigned int mBGTex = createTexture(menuBGImg);
	menu1BG = createTexturedQuad(1024.0f, 1024.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	menu2BG = createTexturedQuad(1024.0f, 1024.0f, 1.0f, 0.0f, 0.0f, 1.0f);

	mapName = (char*)malloc(32);
	memcpy(mapName, "src/games/sokoban/maps/map0.map", 32);
	Map map = loadMap(mapName);

	levelText = (char*)malloc(9);
	memcpy(levelText, "Level: 1", 9);
	float levelTextBoxPosition[]{ 0.01f,0.36f };

	timerText = (char*)malloc(15);
	memcpy(timerText, "Time: 0.000s", 13);
	float timerTextBoxPosition[]{ 0.01f,0.48f };

	stepsText = (char*)malloc(15);
	memcpy(stepsText, "Steps: 0", 12);
	float stepsTextBoxPosition[]{ 0.01f,0.6f };

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

	timerDisplayTimer = timerStartTime = getTime();
	
	while (!win.shouldClose)
	{
		updateTextureGrid(gameGrid);

		clearFrameBuffer();


		useShader(basic);
		drawBasicGrid(floorGrid,-(2.0f/1920.0f)*24.0f,0.001f,0.1f);

		useShader(sokobanShader);
		bindTexture(tex);
		bindVec2(sokobanShader.ID, "trans", adjust);
		bindFloat(sokobanShader.ID, "layerPosition", 0.0f);
		drawGeometry(gameGrid.geom);

		bindTexture(mBGTex);
		float mbgPos[2]{ -0.5f,1.25f };
		bindVec2(sokobanShader.ID, "trans", mbgPos);
		bindFloat(sokobanShader.ID, "layerPosition", -0.1f);
		drawGeometry(menu1BG.geom);
		mbgPos[1] -= 2.1f;
		mbgPos[0] += 0.39f;
		bindVec2(sokobanShader.ID, "trans", mbgPos);
		bindFloat(sokobanShader.ID, "layerPosition", -0.15f);
		drawGeometry(menu2BG.geom);

		useShader(textShader);
		drawTextBox(titleTextBox, 0.1f, 0.12f);
		drawTextBox(highscoresTextBox, 0.28f, 1.11f);
		drawTextBox(hsLevelTextBox, 0.027f, 1.33f); 
		drawTextBox(hsStepsTextBox, 0.343f, 1.33f);
		drawTextBox(hsTimeTextBox, 0.627f, 1.33f);

		for (int i = 0; i < LEVELS; ++i)
		{
			float pos[2]{ hsTableXPositions[0], hsTableYPositions[i] };
			bindVec2(textShader.ID, "trans", pos);
			drawGeometry(hsLevelsTextBoxes[i].geom);
			pos[0] = hsTableXPositions[1];
			bindVec2(textShader.ID, "trans", pos);
			drawGeometry(hsStepsTextBoxes[i].geom);
			pos[0] = hsTableXPositions[2];
			bindVec2(textShader.ID, "trans", pos);
			drawGeometry(hsTimesTextBoxes[i].geom);

		}
		

		bindVec2(textShader.ID, "trans", levelTextBoxPosition);
		drawGeometry(levelTextBox.geom);

		bindVec2(textShader.ID, "trans", timerTextBoxPosition);
		drawGeometry(timerTextBox.geom);

		bindVec2(textShader.ID, "trans", stepsTextBoxPosition);
		drawGeometry(stepsTextBox.geom);


		updateState(win, state);
		updateWindow(win);
	}
}