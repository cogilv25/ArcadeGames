#define AG_GAME_DLL
#include <ABI.h>
#include <engine.h>
#include <stdio.h>
#include <cmath>
#include <time.h>

#define MAX_SCORE 999999

char* dynamicTextFields;

char* scoreTextField;
unsigned int score;
DynamicText scoreText;

float screenSpaceMatrix[]
{ 
	2.0f/1920.0f,0.0f,0.0f,-1.0f,
	0.0f,2.0f/1080.0f,0.0f,-1.0f,
	0.0f,0.0f,1.0f,0.0f,
	0.0f,0.0f,0.0f,1.0f
};

void updateScore()
{
	score = score > MAX_SCORE ? MAX_SCORE : score;
	sprintf(scoreTextField,"Score: %u", score);

	setDynamicTextContents(scoreText, scoreTextField);
}

void rotateActivePiece(float radians)
{
	
}


void updateState(Window& win)
{
	//TODO: Input
	//TODO: De-bouncing
	if (getKeyDown(win, KEY_A) || getKeyDown(win, KEY_LEFT))
	{

	}
	if (getKeyDown(win, KEY_D) || getKeyDown(win, KEY_RIGHT))
	{

	}
	if (getKeyDown(win, KEY_S) || getKeyDown(win, KEY_DOWN))
	{

	}
	
	if (getKeyDown(win, KEY_ESCAPE))
	{
		win.shouldClose = true;
	}
}

void dllMain(Window& win)
{
	Shader tetrisShader = createShader("src/games/tetris/shaders/main.vs", "src/games/tetris/shaders/main.fs");
	const Shader& textShader = getEngineShader(TEXT);

	Font font = loadFont("D:/AG/assets/Spectral.ttf");

	scoreText = createDynamicText(font, 9);

	useShader(tetrisShader);
	bindMat4(tetrisShader.ID, "mat", screenSpaceMatrix);
	useShader(textShader);
	bindMat4(textShader.ID, "mat", screenSpaceMatrix);
	
	unsigned int maxScoreDigits = (unsigned int)log(MAX_SCORE) + 1;
	dynamicTextFields = (char*)calloc(7 + maxScoreDigits + 1, sizeof(char));
	scoreTextField = dynamicTextFields;
	score = 0;
	updateScore();

	time_t curClock = clock() + rand() % 1300 + 500;
	unsigned int range = 700, min = 300;
	while (!win.shouldClose)
	{
		clearFrameBuffer();

		useShader(tetrisShader);
		//Draw Tetris Pieces

		useShader(textShader);
		drawDynamicText(scoreText, -0.1f, 0.9f, -0.2f, 1.0f, 1.0f, 1.0f, 1.0f);

		updateState(win);
		updateWindow(win);
		time_t curTime = clock();
		if (curTime > curClock)
		{
			curClock = curTime + rand() % range + min;
			score += rand() % 4;
			updateScore();
		}
	}

	free(dynamicTextFields);

	destroyDynamicText(scoreText);
	destroyFont(font);

	destroyShader(tetrisShader);
}