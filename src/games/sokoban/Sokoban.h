#define AG_GAME_DLL
#include "ABI.h"
#include "Map.h"
#include "Graphics.h"

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