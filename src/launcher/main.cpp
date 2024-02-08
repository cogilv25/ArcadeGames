#define AG_LAUNCHER
#include "binary_interface.h"
#include "utilities.h"
#include <filesystem>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#define MAX_GAMES 64

HINSTANCE loadedGames[MAX_GAMES] { 0 };

void freeGameDlls()
{
    for (int i = 0; i < MAX_GAMES; ++i)
    {
        if (loadedGames[i] == NULL)
            continue;
        FreeLibrary(loadedGames[i]);
    }
}

struct _GameInfo
{
    char path[MAX_PATH];
    char name[128];
} typedef GameInfo;

struct GameList
{
    GameInfo * games;
    unsigned int count;
};

GameList initGameList()
{
    const char* path =
#if defined(_DEBUG) || defined(__DEBUG)
        "bin/Debug/games/";
#elif defined(_RELEASE)
        "bin/Release/games/";
#elif defined(_PRODUCTION)
        "games/";
#endif

    struct GameList games { (GameInfo*)calloc(MAX_GAMES, sizeof(GameInfo)), 0 };

    WIN32_FIND_DATAA findFileData;
    char searchPath[MAX_PATH];
    sprintf(searchPath, "%s*.dll", path);

    HANDLE hFind = FindFirstFileA(searchPath, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("Error opening directory: %s\n", path);
        return games;
    }

    printf("Loading game dlls from: %s\n", path);

    do
    {
        if (games.count >= MAX_GAMES)
        {
            fprintf(stderr, "Too many games for internal buffer, max supported = %d\n", MAX_GAMES);
            break;
        }

        // Skip directories
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        char* gamePath = games.games[games.count].path;
        char* gameName = games.games[games.count].name;

        sprintf(gamePath, "%s%s", path, findFileData.cFileName);

        const char* fileName = strrchr(gamePath, '\\');
        if (fileName++ == NULL)
            fileName = strrchr(gamePath, '/') + 1;

        size_t count = strlen(fileName) - 4;
        
        if (count > 127)
        {
            fprintf(stderr, "Game name too long (max 127 characters): %s\n", fileName);
            continue;
        }
        strncpy(gameName, fileName, count);

        printf("%s: %s\n", gameName, gamePath);

        games.count++;
    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);

    printf("Done.\n\n");

    return games;
}

void destroyGameList(GameList& list)
{
    free(list.games);
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

    GameList list = initGameList();
    StaticText* gameTexts = (StaticText*)malloc(sizeof(StaticText) * list.count);
    for(int i = 0; i < list.count; ++i)
    {
        gameTexts[i] = createStaticText(list.games[i].name, font);
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

        for (int i = 0; i < list.count; ++i)
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
                runGameDLL(win, list.games[selectedGame - 1].path, loadedGames[selectedGame - 1]);
                win.shouldClose = false;
                ignoreEscape = true;
            }
        }

        if (getMouseButtonDown(win, true))
            clicking = true;

    }

    destroyGameList(list);
    freeGameDlls();
	return 0;
}