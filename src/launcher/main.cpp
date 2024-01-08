#include <iostream>
#define AG_LAUNCHER
#include "binary_interface.h"

int main()
{
    Window win {"Arcade Games Launcher"};
    createWindow(win);
    initializeEngine(win);
    Shader textShader = createShader("src/shaders/text.vs", "src/shaders/text.fs");

    BitmapFont font = loadFontBitmap("D:/AG/assets/Spectral.ttf");

    TextBox tb1 = createTextBox("Press A to run DanceFloor from a dll!", 1.0f, font, 0.86f, 0.86f, 0.86f);
    TextBox tb2 = createTextBox("Press A again to quit!", 1.0f, font, 0.86f, 0.86f, 0.86f);

    bool oneUse = true;
    while (!win.shouldClose)
    {
 
        updateWindow(win);

        clearFrameBuffer();

        useShader(textShader);
        drawTextBox(oneUse ? tb1 : tb2, 0.5f, 0.5f);

        if (getKeyDown(win, 65))
        {
            if (oneUse)
            {
                oneUse = false;
                runGameDLL(win, "DanceFloor.dll");
                win.shouldClose = false;
            }
            else
                win.shouldClose = true;
        }
    }

	return 0;
}