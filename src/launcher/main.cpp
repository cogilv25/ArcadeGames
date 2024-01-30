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
    const Shader& textShader = getEngineShader(TEXT);

    Font font = loadSharedFont("Spectral", "assets/Spectral.ttf");
	if (!font.valid)
		std::cerr << "Could not load font @ assets/Spectral.ttf\n";

    StaticText titleText = createStaticText("Choose a game to play from the list:", font);

    GameList games = initGameList();
    StaticText* gameTexts = (StaticText*)malloc(sizeof(StaticText) * games.count);
    for(int i = 0; i < games.count; ++i)
    {
        gameTexts[i] = createStaticText(games.names[i], font);
    }

    bool clicking = false;
    bool ignoreEscape = true;
    unsigned int selectedGame = 0;
    while (!win.shouldClose)
    {
        updateWindow(win);

        clearFrameBuffer();

        drawStaticText(titleText,-0.5f, 0.88f, -0.2f, 1.0f, 1.0f, 1.0f, 1.0f);

        useShader(textShader);
        float yoff = 0.725f, ygap = -0.133f;

        for (int i = 0; i < games.count; ++i)
            drawStaticText(gameTexts[i], -0.99f, yoff + ygap * i, 0.0f, 1.0f, 1.0f, 1.0f, (i+1==selectedGame)?1.0f:0.75f);

        
        if (ignoreEscape)
        {
            if (!getKeyDown(win, 256))
                ignoreEscape = false;
        }
        else if (getKeyDown(win, 256))
            win.shouldClose = true;

        

        CursorPos cPos = getCursorPosition(win);
        float cPosOffset = -0.04f;
        selectedGame = (unsigned int)( (cPos.y - 0.04f) / -ygap);

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