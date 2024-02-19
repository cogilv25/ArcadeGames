#include "Sokoban.h"
#include <memory>
#include <iostream>

#define LEVELS 5
#define MAX_LEVELS 99
#define MAX_TIME 600
#define MAX_STEPS 999

char* mapName;
char* levelText;
char* timerText;
char* stepsText;
DynamicText levelTextBox;
DynamicText timerTextBox;
DynamicText stepsTextBox;
unsigned int steps;
double timerStartTime;
double timerDisplayTimer;
double levelTime;

bool highscoresVisible = false;
double hsTimes[LEVELS] { 1.843, 5.820, 4.025, 33.019, 40.428 };
unsigned int hsSteps[LEVELS] { 15, 31, 23, 149, 175 };
unsigned int highscoresAchieved = 0;
DynamicText hsTimesTextBoxes[LEVELS];
DynamicText hsStepsTextBoxes[LEVELS];
DynamicText hsLevelsTextBoxes[LEVELS];

char* dynamicText2;
char* hsTextFields[3][LEVELS];

float hsTableXPositions[] { -0.95f, -0.6437f, -0.367f };
float hsTableYPositions[]{ -0.45f, -0.57f, -0.69f, -0.81f, -0.93f };
float hsColours[][3]{
	{ 1.0f, 1.0f, 1.0f },									//Default - White
	{ 219.0f / 255.0f, 188.0f / 255.0f, 35.0f / 255.0f },	//New Record - Gold
	{ 0.0f, 135.0f / 255.0f, 1.0f }							//Level Complete - Blue
};
unsigned char hsTableColours[][LEVELS]{
	{ 0, 0, 0, 0, 0 }, //Levels
	{ 0, 0, 0, 0, 0 }, //Steps
	{ 0, 0, 0, 0, 0 }  //Times
};

float hsTimesColours[LEVELS * 3];
float hsStepsColours[LEVELS * 3];

float screenSpaceMatrix[]
{ 
	2.0f/1920.0f,0.0f,0.0f,-1.0f,
	0.0f,2.0f/1080.0f,0.0f,-1.0f,
	0.0f,0.0f,1.0f,0.0f,
	0.0f,0.0f,0.0f,1.0f
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
TexturedQuad menu1BG;
TexturedQuad menu2BG;


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
		fprintf(stderr, "Map is too wide!\n");
		return false;
	}
	else if (map.width + map.xOff > grid.width)
	{
		fprintf(stderr, "Map is off screen, adjusting and continuing..\n");
		map.xOff = grid.width - map.width;
	}

	if (map.height > grid.width)
	{
		fprintf(stderr, "Map is too tall!\n");
		return false;
	}
	else if (map.height + map.yOff > grid.height)
	{
		fprintf(stderr, "Map is off screen, adjusting and continuing..\n");
		map.yOff = grid.height - map.height;
	}

	initMapNoChecks(map, grid, state);
	return true;
}

bool initMapCentred(Map& map, TextureGrid& grid, State& state)
{
	if (map.width > grid.width)
	{
		fprintf(stderr, "Map is too wide!\n");
		return false;
	}

	if (map.height > grid.width)
	{
		fprintf(stderr, "Map is too tall!\n");
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
	unsigned int tileWidth = width / (float)xCells;
	unsigned int tileHeight = height / (float)yCells;

	//Create a concatenated list of all x points then all y points
	float* xPoints = (float*)malloc((xCells + 1) + (yCells + 1) * sizeof(float));
	float* yPoints = xPoints + xCells + 1;

	for (int i = 0; i < xCells + 1; ++i)
		xPoints[i] = tileWidth * i;
	
	for(int j = 0; j < yCells + 1; ++j)
		yPoints[j] = tileHeight * j;

	BTVertex* vertices = (BTVertex*)malloc(xCells * yCells * 4 * sizeof(BTVertex));


	for (int j = 0; j < yCells; j++)
		for (int i = 0; i < xCells; i++)
		{
			int n = (j * xCells + i) * 4;

			vertices[n].x = xPoints[i]; vertices[n].y = yPoints[j];
			vertices[n].s = valueToUVMap[0].t1; vertices[n].t = valueToUVMap[0].s1;

			vertices[n + 1].x = xPoints[i + 1]; vertices[n + 1].y = yPoints[j];
			vertices[n + 1].s = valueToUVMap[0].t0; vertices[n + 1].t = valueToUVMap[0].s0;

			vertices[n + 2].x = xPoints[i + 1]; vertices[n + 2].y = yPoints[j + 1];
			vertices[n + 2].s = valueToUVMap[0].t0; vertices[n + 2].t = valueToUVMap[0].s0;

			vertices[n + 3].x = xPoints[i]; vertices[n + 3].y = yPoints[j + 1];
			vertices[n + 3].s = valueToUVMap[0].t1; vertices[n + 3].t = valueToUVMap[0].s0;
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
	TextureGrid out { geom, (unsigned char*)calloc(xCells * yCells,sizeof(unsigned char)), xCells, yCells, xPoints, yPoints };
	return out;
}

void destroyTextureGrid(TextureGrid& grid)
{
	free(grid.grid);
	free(grid.geom.vertices);
	free(grid.geom.indices);
	free(grid.xPoints);
}

void updateHighScore(unsigned int level, double time, unsigned int steps)
{
	bool newTime = hsTimes[level] > time;
	bool sameOrNewTime = hsTimes[level] >= time;
	bool newSteps = hsSteps[level] > steps;
	bool sameOrNewSteps = hsSteps[level] >= steps;

	if (newTime)
	{
		hsTimes[level] = time;
		sprintf(hsTextFields[2][level], "%.3fs", hsTimes[level]);
		setDynamicTextContents(hsTimesTextBoxes[level], hsTextFields[2][level]);
		hsTableColours[2][level] = 1;
	}
	if (newSteps)
	{
		hsSteps[level] = steps;
		sprintf(hsTextFields[1][level], "%d", hsSteps[level]);
		setDynamicTextContents(hsStepsTextBoxes[level], hsTextFields[1][level]);
		hsTableColours[1][level] = 1;
	}


	hsTableColours[0][level] = ((unsigned char)sameOrNewTime + (unsigned char)sameOrNewSteps)/2*2;

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
	setDynamicTextContents(stepsTextBox, stepsText);

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
			setDynamicTextContents(stepsTextBox, stepsText);
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
			setDynamicTextContents(levelTextBox, levelText);

			clearTextureGrid(*state.grid, 0);
			destroyMap(*state.map);
			*state.map = createMap(mapName);
			initMapCentred(*state.map, *state.grid, state);

			sprintf(stepsText, "Steps: %d", steps = 0);
			setDynamicTextContents(stepsTextBox, stepsText);
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

	//Only modify the text that needs modified
	sprintf(timerText + 6, "%.3fs", levelTime);
	setDynamicTextContents(timerTextBox, timerText);

	if (state.nObj == state.completedObj)
	{
		updateHighScore(state.level, levelTime, steps);

		++state.level %= state.nLevels;
		mapName[26] = (char)(state.level+48);
		levelText[7] = mapName[26] + 1;
		setDynamicTextContents(levelTextBox, levelText);

		clearTextureGrid(*state.grid, 0);
		destroyMap(*state.map);
		*state.map = createMap(mapName);
		initMapCentred(*state.map, *state.grid, state);

		sprintf(stepsText, "Steps: %d", steps = 0);
		setDynamicTextContents(stepsTextBox, stepsText);
	}
	if (getKeyDown(win, 256))
	{
		win.shouldClose = true;
	}
}

void dllMain(Window& win)
{

	Shader basic = createShader("src/games/sokoban/shaders/basic.vs", "src/games/sokoban/shaders/basic.fs");
	Shader sokobanShader = createShader("src/games/sokoban/shaders/main.vs", "src/games/sokoban/shaders/main.fs");
	const Shader& textShader2 = getEngineShader(TEXT);

	Font font = loadFont("D:/AG/assets/Spectral.ttf");

	StaticText titleTextBox = createStaticText("Sokoban", font);
	StaticText highscoresTextBox = createStaticText("Highscores", font);
	StaticText hsLevelTextBox = createStaticText("Level", font);
	StaticText hsStepsTextBox = createStaticText("Steps", font);
	StaticText hsTimeTextBox = createStaticText("Time", font);

	levelTextBox = createDynamicText(font, 9);
	setDynamicTextContents(levelTextBox, "Level: 1");

	timerTextBox = createDynamicText(font, 15);
	setDynamicTextContents(timerTextBox, "Time: 0.000s");

	stepsTextBox = createDynamicText(font, 12);
	setDynamicTextContents(stepsTextBox, "Steps: 0");


	float color[] = {1.0f,1.0f,1.0f,0.8f,0.8f,0.8f};
	BasicGrid floorGrid = createBasicGrid(1944.0f, 1080.0f, 27, 15, 0.0f, 0.0f, color, 2);
	TextureGrid gameGrid = createTextureGrid(1080.0f, 1080.0f,15,15,0.0f,0.0f);

	Image im = loadImage("assets/test.png");
	unsigned int tex = createTexture(im);
	destroyImage(im);

	Image menuBGImg = loadImage("assets/menubgtop.png");
	unsigned int mBGTex = createTexture(menuBGImg);
	menu1BG = createTexturedQuad(1024.0f, 1024.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	menu2BG = createTexturedQuad(1024.0f, 1024.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	destroyImage(menuBGImg);

	char* dynamicTextBuffer = (char*)malloc(69);

	mapName = dynamicTextBuffer;
	memcpy(mapName, "src/games/sokoban/maps/map0.map", 32);
	Map map = createMap(mapName);

	levelText = mapName + 33;
	memcpy(levelText, "Level: 1", 9);
	float levelTextBoxPosition[]{ 0.01f,0.36f };

	timerText = levelText + 9;
	memcpy(timerText, "Time: 0.000s", 13);
	float timerTextBoxPosition[]{ 0.01f,0.48f };

	stepsText = timerText + 15;
	memcpy(stepsText, "Steps: 0", 9);
	float stepsTextBoxPosition[]{ 0.01f,0.6f };

	State state;
	state.grid = &gameGrid;
	state.nLevels = 5;

	unsigned int hsLevelMaxChars, hsStepsMaxChars, hsTimesMaxChars;
	hsLevelMaxChars = log10(MAX_LEVELS) + 2; //Length == (log10(x) + 1) + 1 (for \0 char)
	hsStepsMaxChars = log10(MAX_STEPS) + 2;  //same as above
	hsTimesMaxChars = log10(MAX_TIME) + 7;   //same as above + room for ".000s" == +5
	unsigned int stride = hsLevelMaxChars + hsStepsMaxChars + hsTimesMaxChars;
	DynamicText* dtTables[3]{ hsLevelsTextBoxes, hsStepsTextBoxes, hsTimesTextBoxes };

	dynamicText2 = (char*)calloc(LEVELS, stride);
	unsigned int textFieldMaxs[3]{ hsLevelMaxChars, hsStepsMaxChars, hsTimesMaxChars };
	for (int i = 0; i < LEVELS; ++i)
	{
		hsTextFields[0][i] = dynamicText2 + i * stride;
		for (int j = 1; j < 3; j++)
			hsTextFields[j][i] = hsTextFields[j-1][i] + textFieldMaxs[j-1];

		sprintf(hsTextFields[0][i], "%d", i);
		sprintf(hsTextFields[1][i], "%d", hsSteps[i]);
		sprintf(hsTextFields[2][i], "%.3fs", hsTimes[i]);

		for (int j = 0; j < 3; j++)
		{
			dtTables[j][i] = createDynamicText(font, textFieldMaxs[j]);
			setDynamicTextContents(dtTables[j][i], hsTextFields[j][i]);
			hsTimesColours[i * 3 + j] = 1.0f;
			hsStepsColours[i * 3 + j] = 1.0f;
		}
	}

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

		useShader(textShader2);
		drawStaticText(titleTextBox, -0.89f, 0.88f,-0.2f,1.0f,1.0f,1.0f,1.0f);
		drawStaticText(highscoresTextBox, -0.71f, -0.11f, -0.2f, 1.0f, 1.0f, 1.0f, 1.0f);
		drawStaticText(hsLevelTextBox, -0.9605f, -0.33f, -0.2f, 1.0f, 1.0f, 1.0f, 1.0f);
		drawStaticText(hsStepsTextBox, -0.6435f, -0.33f, -0.2f, 1.0f, 1.0f, 1.0f, 1.0f);
		drawStaticText(hsTimeTextBox, -0.3668f, -0.33f, -0.2f, 1.0f, 1.0f, 1.0f, 1.0f);
		drawDynamicText(levelTextBox, -0.98f, 0.64f, -0.2f, 1.0f, 1.0f, 1.0f, 1.0f);
		drawDynamicText(timerTextBox, -0.98f, 0.52f, -0.2f, 1.0f, 1.0f, 1.0f, 1.0f);
		drawDynamicText(stepsTextBox, -0.98f, 0.4f, -0.2f, 1.0f, 1.0f, 1.0f, 1.0f);

		for (int i = 0; i < LEVELS; ++i)
			for (int j = 0; j < 3; j++)
			{
				float x = hsTableXPositions[j], y = hsTableYPositions[i];
				float* col = hsColours[hsTableColours[j][i]];
				drawDynamicText(dtTables[j][i], x, y, -0.2f, col[0], col[1], col[2], 1.0f);
			}

		updateState(win, state);
		updateWindow(win);
	}

	for(int i = 0; i < LEVELS; ++i )
		for(int j = 0; j < 3; ++j)
			destroyDynamicText(dtTables[j][i]);

	destroyMap(map);

	free(dynamicTextBuffer);
	free(dynamicText2);

	destroyTexturedQuad(menu1BG);
	destroyTexturedQuad(menu2BG);

	destroyTexture(mBGTex);
	destroyTexture(tex);

	destroyTextureGrid(gameGrid);
	destroyBasicGrid(floorGrid);

	destroyDynamicText(levelTextBox);
	destroyDynamicText(stepsTextBox);
	destroyDynamicText(timerTextBox);

	destroyStaticText(titleTextBox);
	destroyStaticText(highscoresTextBox);
	destroyStaticText(hsLevelTextBox);
	destroyStaticText(hsStepsTextBox);
	destroyStaticText(hsTimeTextBox);

	destroyFont(font);

	destroyShader(sokobanShader);
	destroyShader(basic);
}