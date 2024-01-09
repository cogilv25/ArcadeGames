#define AG_GAME_DLL
#include "binary_interface.h"

void entryPoint(Window& win)
{

	Shader textShader = createShader("src/shaders/text.vs", "src/shaders/text.fs");


	BitmapFont font = loadFontBitmap("D:/AG/assets/Spectral.ttf");

	TextBox tb = createTextBox("Sokoban", 1.0f, font, 0.86f, 0.86f, 0.86f);

	while (!win.shouldClose)
	{
		useShader(textShader);
		drawTextBox(tb, 0.85f, 0.15f);

		updateWindow(win);
		if (getKeyDown(win, 256))
		{
			win.shouldClose = true;
		}
		clearFrameBuffer();
	}
}