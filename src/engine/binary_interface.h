/*
    Maintains a consistant ABI between the launcher, engine and games
    Also provides functions for linking any game dll to the final application at runtime.

    TODO: Too awkward and too much repitition, rewrite and/or reorganize.. possibly autogen with a script?
*/
#ifndef AG_BINARY_INTERFACE_H
#define AG_BINARY_INTERFACE_H
#include "engine.h"

#ifdef AG_GAME_DLL
bool(*getKeyDown)(Window&, int key);
void(*updateWindow)(Window&);
void(*clearFrameBuffer)();
void(*drawGeometry)(Geometry&);
Geometry (*createGeometry)(float*, unsigned int*, unsigned int*, unsigned int, unsigned int, unsigned int, unsigned int);
double(*getTime)();
void (*updateGeometry)(Geometry&);
void (*useShader)(const Shader&);
Shader(*createShader)(const char*, const char*);
TextBox(*createTextBox)(const char*, float, Font&, float, float, float);
Font(*loadFont)(const char*);
Font& (*loadSharedFont)(const char*, const char*);
void(*drawTextBox)(TextBox&, float, float);
void (*destroyBitmapFont)(Font&);
CursorPos(*getCursorPosition)(Window& win);
Rect(*createRect)(float, float, float, float, float);
void(*drawRect)(Rect&, float, float);
BasicGrid(*createBasicGrid)(float, float, unsigned int, unsigned int, float, float, float*, unsigned int);
void (*drawBasicGrid)(BasicGrid& grid, float x, float y, float z);
unsigned int (*createTexture)(Image&);
void (*destroyTexture)(unsigned int ID);
Image (*loadImage)(const char* path);
void (*destroyImage)(Image&);
void (*bindMat4)(unsigned int shaderID, const char* name, float* matrix);
void (*bindVec2)(unsigned int shaderID, const char* name, float* vector);
void (*bindTexture)(unsigned int id);
void (*bindFloat)(unsigned int shaderID, const char* name, float f);
StaticText (*createStaticText)(const char* text, Font&);
void (*drawStaticText)(StaticText&, float x, float y, float z, float r, float g, float b, float a);
const Shader& (*getEngineShader)(AG_SHADER shader);
DynamicText (*createDynamicText)(Font&, unsigned int maxCharacters);
void (*setDynamicTextContents)(DynamicText&, const char* contents);
void (*drawDynamicText)(DynamicText&, float x, float y, float z, float r, float g, float b, float a);

void dllMain(Window& win);
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
        useShader = (void(*)(const Shader&))engineFunctions[7];
        createShader = (Shader(*)(const char*, const char*))engineFunctions[8];
        createTextBox = (TextBox(*)(const char*, float, Font&, float, float, float))engineFunctions[9];
        loadFont = (Font(*)(const char*))engineFunctions[10];
        loadSharedFont = (Font&(*)(const char*, const char*))engineFunctions[11];
        drawTextBox = (void(*)(TextBox&, float, float))engineFunctions[12];
        destroyBitmapFont = (void (*)(Font&))engineFunctions[13];
        getCursorPosition = (CursorPos(*)(Window & win))engineFunctions[14];
        createRect = (Rect(*)(float, float, float, float, float))engineFunctions[15];
        drawRect = (void(*)(Rect&,float,float))engineFunctions[16];
        createBasicGrid = (BasicGrid(*)(float, float, unsigned int, unsigned int, float, float, float*, unsigned int))engineFunctions[17];
        drawBasicGrid = (void (*)(BasicGrid & grid, float x, float y, float z))engineFunctions[18];
        createTexture = (unsigned int(*)(Image&))engineFunctions[19];
        destroyTexture = (void (*)(unsigned int))engineFunctions[20];
        loadImage = (Image(*)(const char* path))engineFunctions[21];
        destroyImage = (void (*)(Image&))engineFunctions[22];
        bindMat4 = (void(*)(unsigned int, const char*, float*))engineFunctions[23];
        bindVec2 = (void(*)(unsigned int, const char*, float*))engineFunctions[24];
        bindTexture = (void(*)(unsigned int))engineFunctions[25];
        bindFloat = (void(*)(unsigned int, const char*, float))engineFunctions[26];
        createStaticText = (StaticText(*)(const char*, Font&))engineFunctions[27];
        drawStaticText = (void(*)(StaticText&,float,float,float,float,float,float,float))engineFunctions[28];
        getEngineShader = (const Shader& (*)(AG_SHADER))engineFunctions[29];
        createDynamicText = (DynamicText(*)(Font&,unsigned int))engineFunctions[30];
        setDynamicTextContents = (void(*)(DynamicText&,const char*))engineFunctions[31];
        drawDynamicText = (void(*)(DynamicText&,float,float,float,float,float,float,float))engineFunctions[32];

        dllMain(win);
    }
}
#endif
#ifdef AG_LAUNCHER
#include <windows.h>


bool runGameDLL(Window& win, const char* path)
{
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
        (void(*)()) loadFont,
        (void(*)()) loadSharedFont,
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
        (void(*)()) destroyImage,
        (void(*)()) bindMat4,
        (void(*)()) bindVec2,
        (void(*)()) bindTexture,
        (void(*)()) bindFloat,
        (void(*)()) createStaticText,
        (void(*)()) drawStaticText,
        (void(*)()) getEngineShader,
        (void(*)()) createDynamicText,
        (void(*)()) setDynamicTextContents,
        (void(*)()) drawDynamicText
    };

    entry(win, fun);

    FreeLibrary(dll);
    return true;
}

#endif

#endif