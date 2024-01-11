/*
    Maintains a consistant ABI between the launcher, engine and games
    Also provides functions for linking any game dll to the final application at runtime.

    TODO: Too awkward and too much repitition, rewrite and/or reorganize.. possibly autogen with a script?
*/
#ifndef AG_BINARY_INTERFACE_H
#define AG_BINARY_INTERFACE_H

struct Window
{
    const char* title = "Hello World";
    int glVersion[2] = { 4, 6 };
    bool glCoreProfile = true;
    int width = 1920;
    int height = 1080;
    bool fullscreen = false;
    bool borderless = false;
    bool shouldClose = false;
    void* glfwRef = 0;
};

struct CursorPos
{
    double x, y;
};

struct BasicVertex
{
    float x, y;
    float r, g, b;
};

struct Rect
{
    unsigned int vao;
    unsigned int vbo;
    unsigned int ibo;

    float vertices[4*5];
    unsigned int indices[6];
};

struct Image
{
    int width, height;
    unsigned char* data;
};

struct Geometry
{
    unsigned int vao;
    unsigned int vbo;
    unsigned int ibo;

    float* vertices;
    unsigned int* indices;

    unsigned int vertexCount;
    unsigned int indexCount;
    unsigned int stride;
};

struct BasicGrid
{
    float width;
    float height;
    unsigned int xCells;
    unsigned int yCells;
    float xGap;
    float yGap;
    float* colors;
    unsigned int colorCount;

    Geometry geom;
};

struct Shader
{
    unsigned int ID;
    bool valid;
};

struct BitmapCharData
{
    unsigned short x0, y0, x1, y1; // bounding box
    float xoff, yoff, xadvance; // offset from baseline, distance to origin of next char
};

struct UIVertex
{
    float x, y;		// positions
    float r, g, b;	// colour
    float s, t;		// texture coords
};

struct BitmapFont
{
    unsigned int ID;
    unsigned int width, height;
    BitmapCharData* cdata;
};

struct TextBox
{
    //Number of characters
    unsigned int length;
    //OpenGL buffers
    unsigned int vao;
    unsigned int vbo;
    unsigned int ibo;

    UIVertex* vertices;
    BitmapFont* font;
};


#ifdef AG_GAME_DLL
Window window;

bool(*getKeyDown)(Window&, int key);
void(*updateWindow)(Window&);
void(*clearFrameBuffer)();
void(*drawGeometry)(Geometry&);
Geometry (*createGeometry)(float*, unsigned int*, unsigned int*, unsigned int, unsigned int, unsigned int, unsigned int);
double(*getTime)();
void (*updateGeometry)(Geometry&);
void (*useShader)(Shader&);
Shader(*createShader)(const char*, const char*);
TextBox(*createTextBox)(const char*, float, BitmapFont&, float, float, float);
BitmapFont(*loadFontBitmap)(const char*);
void(*drawTextBox)(TextBox&, float, float);
void (*destroyBitmapFont)(BitmapFont&);
CursorPos(*getCursorPosition)(Window& win);
Rect(*createRect)(float, float, float, float, float);
void(*drawRect)(Rect&, float, float);
BasicGrid(*createBasicGrid)(float, float, unsigned int, unsigned int, float, float, float*, unsigned int);
void (*drawBasicGrid)(BasicGrid& grid, float x, float y, float z);
unsigned int (*createTexture)(Image&);
void (*destroyTexture)(unsigned int ID);
Image (*loadImage)(const char* path);
void (*destroyImage)(Image&);

void entryPoint(Window& win);
extern "C"
{
    __declspec(dllexport) void __cdecl runGame(Window& win, void(**engineFunctions)())
    {
        getKeyDown = (bool(*)(Window&, int))engineFunctions[0];
        updateWindow = (void(*)(Window&))engineFunctions[1];
        clearFrameBuffer = engineFunctions[2];
        drawGeometry = (void(*)(Geometry&))engineFunctions[3];
        createGeometry = (Geometry(*)(float*, unsigned int*, unsigned int*, unsigned int, unsigned int, unsigned int, unsigned int))engineFunctions[4];
        getTime = (double(*)())engineFunctions[5];
        updateGeometry = (void(*)(Geometry&))engineFunctions[6];
        useShader = (void(*)(Shader&))engineFunctions[7];
        createShader = (Shader(*)(const char*, const char*))engineFunctions[8];
        createTextBox = (TextBox(*)(const char*, float, BitmapFont&, float, float, float))engineFunctions[9];
        loadFontBitmap = (BitmapFont(*)(const char*))engineFunctions[10];
        drawTextBox = (void(*)(TextBox&, float, float))engineFunctions[11];
        destroyBitmapFont = (void (*)(BitmapFont&))engineFunctions[12];
        getCursorPosition = (CursorPos(*)(Window & win))engineFunctions[13];
        createRect = (Rect(*)(float, float, float, float, float))engineFunctions[14];
        drawRect = (void(*)(Rect&,float,float))engineFunctions[15];
        createBasicGrid = (BasicGrid(*)(float, float, unsigned int, unsigned int, float, float, float*, unsigned int))engineFunctions[16];
        drawBasicGrid = (void (*)(BasicGrid & grid, float x, float y, float z))engineFunctions[17];
        createTexture = (unsigned int(*)(Image&))engineFunctions[18];
        destroyTexture = (void (*)(unsigned int))engineFunctions[19];
        loadImage = (Image(*)(const char* path))engineFunctions[20];
        destroyImage = (void (*)(Image&))engineFunctions[21];

        entryPoint(win);
    }
}
#endif
#ifdef AG_LAUNCHER
#include "engine.h"
#ifdef _WIN32
#include <windows.h>
#endif


bool runGameDLL(Window& win, const char* path)
{
#ifdef _WIN32
    HINSTANCE dll;
    void(*entry)(Window&,void(**)());

    dll = LoadLibraryA(path);

    if (dll == NULL)
        return false;

    entry = (void(*)(Window&, void(**)())) GetProcAddress(dll, "runGame");

    if (entry == NULL)
        return false;

    void(*fun[])() 
    { 
        (void(*)()) getKeyDown,
        (void(*)()) updateWindow,
        clearFrameBuffer,
        (void(*)()) drawGeometry,
        (void(*)()) createGeometry,
        (void(*)()) getTime,
        (void(*)()) updateGeometry,
        (void(*)()) useShader,
        (void(*)()) createShader,
        (void(*)()) createTextBox,
        (void(*)()) loadFontBitmap,
        (void(*)()) drawTextBox,
        (void(*)()) destroyBitmapFont,
        (void(*)()) getCursorPosition,
        (void(*)()) createRect,
        (void(*)()) drawRect,
        (void(*)()) createBasicGrid,
        (void(*)()) drawBasicGrid,
        (void(*)()) createTexture,
        (void(*)()) destroyTexture,
        (void(*)()) loadImage,
        (void(*)()) destroyImage

    };

    entry(win, fun);

    FreeLibrary(dll);
    return true;
#endif
}

#endif

#endif