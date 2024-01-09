#include <iostream>
#define AG_LAUNCHER
#include "binary_interface.h"
#include "utilities.h"
#include <filesystem>
#include <iostream>

struct GameList
{
    char* paths[64];
    char* names[64];
    unsigned int count;
};

GameList initGameList()
{
#if defined(_DEBUG) or defined(__DEBUG)
    const char* path = "bin/Debug/games/";
#elif defined(_RELEASE)
    const char* path = "bin/Release/games/";
#elif defined(_PRODUCTION)
    const char* path = "games/";
#endif

    GameList games{ 0 };

    IFDBG(std::cout << "Enumerating Games..\n");

    unsigned int* n = &games.count;
    for (auto const& dir_entry : std::filesystem::directory_iterator{ path })
    {
        //Extract Name 
        std::string gamePath = dir_entry.path().string().c_str();
        unsigned int pathLength = gamePath.length();
        unsigned int startIndex = gamePath.find_last_of('/') + 1;
        unsigned int nameLength = pathLength - startIndex - 4;

        games.paths[*n] = (char*)malloc(pathLength + 1);
        games.names[*n] = (char*)malloc(nameLength + 1);
        games.names[*n][nameLength] = 0; games.paths[*n][pathLength] = 0;

        memcpy(games.paths[*n], gamePath.c_str(), pathLength);
        memcpy(games.names[*n], gamePath.c_str() + startIndex, nameLength);

        IFDBG(std::cout << games.names[*n] << ": " << games.paths[*n] << "\n");
        ++(*n);
        if (*n > 63)
        {
            std::cerr << "\nToo many games for internal buffer, max supported = 64\n";
            break;
        }
    }
    IFDBG(std::cout << "Done.\n");
    return games;
}

int main()
{
    Window win{ "Arcade Games Launcher" };
    createWindow(win);
    initializeEngine(win);
    Shader textShader = createShader("src/shaders/text.vs", "src/shaders/text.fs");

    BitmapFont font = loadFontBitmap("D:/AG/assets/Spectral.ttf");

    TextBox titleTextBox = createTextBox("Choose a game to play from the list:", 1.0f, font, 1.0f, 1.0f, 1.0f);

    GameList games = initGameList();
    TextBox* gameTextBoxes = (TextBox*)malloc(sizeof(TextBox) * games.count * 2);
    for(int i = 0; i < games.count; ++i)
    {
        gameTextBoxes[i*2] = createTextBox(games.names[i], 1.0f, font, 0.75f, 0.75f, 0.75f);
        gameTextBoxes[i*2+1] = createTextBox(games.names[i], 1.0f, font, 1.0f, 1.0f, 1.0f);
    }


    bool clicking = false;
    bool ignoreEscape = false;
    unsigned int selectedGame = 0;
    while (!win.shouldClose)
    {

        updateWindow(win);

        clearFrameBuffer();

        useShader(textShader);
        drawTextBox(titleTextBox, 0.5f, 0.1f);
        float yoff = 0.275f, ygap = 0.133f;
        for (int i = 0; i < games.count; ++i)
        {
            drawTextBox(selectedGame != i + 1 ? gameTextBoxes[i * 2] : gameTextBoxes[i * 2 + 1], 0.0f, yoff + (ygap * i));
        }
        if (ignoreEscape)
        {
            if (!getKeyDown(win, 256))
                ignoreEscape = false;
        }
        else if (getKeyDown(win, 256))
            win.shouldClose = true;

        

        CursorPos cPos = getCursorPosition(win);
        if (cPos.y < yoff - 0.1f || cPos.y > yoff + ygap * (games.count - 1))
            selectedGame = 0;
        else
        {
            selectedGame = (unsigned int)((cPos.y - yoff + 0.1f) / ygap)+1;
        }

        if(clicking && !getMouseButtonDown(win,true))
        {
            clicking = false;
            if (selectedGame != 0)
            {
                runGameDLL(win, games.paths[selectedGame - 1]);
                win.shouldClose = false;
                ignoreEscape = true;
            }
        }

        if (getMouseButtonDown(win, true))
            clicking = true;

    }

	return 0;
}