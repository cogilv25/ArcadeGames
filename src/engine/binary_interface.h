// AG_BinaryInterface.h

#ifndef AG_BINARY_INTERFACE_H
#define AG_BINARY_INTERFACE_H

#include "engine.h"


typedef void (*VoidFunc)();

// Function pointer types for engine functions
typedef bool (*GetKeyDownFunc)(Window&, int);
typedef void (*UpdateWindowFunc)(Window&);
typedef void (*ClearFrameBufferFunc)();
typedef void (*DrawGeometryFunc)(Geometry&);
typedef Geometry(*CreateGeometryFunc)(float*, unsigned int*, unsigned int*, unsigned int, unsigned int, unsigned int, unsigned int);
typedef double (*GetTimeFunc)();
typedef void (*UpdateGeometryFunc)(Geometry&);
typedef void (*UseShaderFunc)(const Shader&);
typedef Shader(*CreateShaderFunc)(const char*, const char*);
typedef Font(*LoadFontFunc)(const char*);
typedef Font& (*LoadSharedFontFunc)(const char*, const char*);
typedef void (*DestroyFontFunc)(Font&);
typedef CursorPos(*GetCursorPositionFunc)(Window&);
typedef BasicGrid(*CreateBasicGridFunc)(float, float, unsigned int, unsigned int, float, float, float*, unsigned int);
typedef void (*DrawBasicGridFunc)(BasicGrid&, float x, float y, float z);
typedef unsigned int (*CreateTextureFunc)(Image&);
typedef void (*DestroyTextureFunc)(unsigned int);
typedef Image(*LoadImageFunc)(const char* path);
typedef void (*DestroyImageFunc)(Image&);
typedef void (*BindMat4Func)(unsigned int shaderID, const char* name, float* matrix);
typedef void (*BindVec2Func)(unsigned int shaderID, const char* name, float* vector);
typedef void (*BindTextureFunc)(unsigned int id);
typedef void (*BindFloatFunc)(unsigned int shaderID, const char* name, float f);
typedef StaticText(*CreateStaticTextFunc)(const char* text, Font&);
typedef void (*DrawStaticTextFunc)(StaticText&, float x, float y, float z, float r, float g, float b, float a);
typedef const Shader& (*GetEngineShaderFunc)(AG_SHADER shader);
typedef DynamicText(*CreateDynamicTextFunc)(Font&, unsigned int);
typedef void (*SetDynamicTextContentsFunc)(DynamicText&, const char*);
typedef void (*DrawDynamicTextFunc)(DynamicText&, float x, float y, float z, float r, float g, float b, float a);
typedef void (*DestroyGeeometryFunc)(Geometry&);
typedef void (*DestroyShaderFunc)(Shader&);
typedef void (*DestroyStaticText)(StaticText&);
typedef void (*DestroyDynamicText)(DynamicText&);
typedef void (*DestroyBasicGridFunc)(BasicGrid&);

#ifdef AG_GAME_DLL

// Function pointers
GetKeyDownFunc getKeyDown;
UpdateWindowFunc updateWindow;
ClearFrameBufferFunc clearFrameBuffer;
DrawGeometryFunc drawGeometry;
CreateGeometryFunc createGeometry;
GetTimeFunc getTime;
UpdateGeometryFunc updateGeometry;
UseShaderFunc useShader;
CreateShaderFunc createShader;
LoadFontFunc loadFont;
LoadSharedFontFunc loadSharedFont;
DestroyFontFunc destroyFont;
GetCursorPositionFunc getCursorPosition;
CreateBasicGridFunc createBasicGrid;
DrawBasicGridFunc drawBasicGrid;
CreateTextureFunc createTexture;
DestroyTextureFunc destroyTexture;
LoadImageFunc loadImage;
DestroyImageFunc destroyImage;
BindMat4Func bindMat4;
BindVec2Func bindVec2;
BindTextureFunc bindTexture;
BindFloatFunc bindFloat;
CreateStaticTextFunc createStaticText;
DrawStaticTextFunc drawStaticText;
GetEngineShaderFunc getEngineShader;
CreateDynamicTextFunc createDynamicText;
SetDynamicTextContentsFunc setDynamicTextContents;
DrawDynamicTextFunc drawDynamicText;
DestroyGeeometryFunc destroyGeometry;
DestroyShaderFunc destroyShader;
DestroyStaticText destroyStaticText;
DestroyDynamicText destroyDynamicText;
DestroyBasicGridFunc destroyBasicGrid;

void dllMain(Window& win, VoidFunc* functionArray)
{

    // Your initialization code for the DLL can go here
    // This function will be called when the DLL is loaded
}

extern "C"
{
    void dllMain(Window&);
    __declspec(dllexport) void __cdecl runGame(Window& win, VoidFunc* functionArray)
    {
        // Initialize function pointers using the array
        getKeyDown = (GetKeyDownFunc)functionArray[0];
        updateWindow = (UpdateWindowFunc)functionArray[1];
        clearFrameBuffer = (ClearFrameBufferFunc)functionArray[2];
        drawGeometry = (DrawGeometryFunc)functionArray[3];
        createGeometry = (CreateGeometryFunc)functionArray[4];
        getTime = (GetTimeFunc)functionArray[5];
        updateGeometry = (UpdateGeometryFunc)functionArray[6];
        useShader = (UseShaderFunc)functionArray[7];
        createShader = (CreateShaderFunc)functionArray[8];
        loadFont = (LoadFontFunc)functionArray[9];
        loadSharedFont = (LoadSharedFontFunc)functionArray[10];
        destroyFont = (DestroyFontFunc)functionArray[11];
        getCursorPosition = (GetCursorPositionFunc)functionArray[12];
        createBasicGrid = (CreateBasicGridFunc)functionArray[13];
        drawBasicGrid = (DrawBasicGridFunc)functionArray[14];
        createTexture = (CreateTextureFunc)functionArray[15];
        destroyTexture = (DestroyTextureFunc)functionArray[16];
        loadImage = (LoadImageFunc)functionArray[17];
        destroyImage = (DestroyImageFunc)functionArray[18];
        bindMat4 = (BindMat4Func)functionArray[19];
        bindVec2 = (BindVec2Func)functionArray[20];
        bindTexture = (BindTextureFunc)functionArray[21];
        bindFloat = (BindFloatFunc)functionArray[22];
        createStaticText = (CreateStaticTextFunc)functionArray[23];
        drawStaticText = (DrawStaticTextFunc)functionArray[24];
        getEngineShader = (GetEngineShaderFunc)functionArray[25];
        createDynamicText = (CreateDynamicTextFunc)functionArray[26];
        setDynamicTextContents = (SetDynamicTextContentsFunc)functionArray[27];
        drawDynamicText = (DrawDynamicTextFunc)functionArray[28];
        destroyGeometry = (DestroyGeeometryFunc)functionArray[29];
        destroyShader = (DestroyShaderFunc)functionArray[30];
        destroyStaticText = (DestroyStaticText)functionArray[31];
        destroyDynamicText = (DestroyDynamicText)functionArray[32];
        destroyBasicGrid = (DestroyBasicGridFunc)functionArray[33];

        //Run the game function defined in the dll code
        dllMain(win);;
    }
}

#endif

#ifdef AG_LAUNCHER

#include <windows.h>



// Define an array of VoidFunc pointers
VoidFunc fun[] =
{
    (VoidFunc)getKeyDown,
    (VoidFunc)updateWindow,
    (VoidFunc)clearFrameBuffer,
    (VoidFunc)drawGeometry,
    (VoidFunc)createGeometry,
    (VoidFunc)getTime,
    (VoidFunc)updateGeometry,
    (VoidFunc)useShader,
    (VoidFunc)createShader,
    (VoidFunc)loadFont,
    (VoidFunc)loadSharedFont,
    (VoidFunc)destroyFont,
    (VoidFunc)getCursorPosition,
    (VoidFunc)createBasicGrid,
    (VoidFunc)drawBasicGrid,
    (VoidFunc)createTexture,
    (VoidFunc)destroyTexture,
    (VoidFunc)loadImage,
    (VoidFunc)destroyImage,
    (VoidFunc)bindMat4,
    (VoidFunc)bindVec2,
    (VoidFunc)bindTexture,
    (VoidFunc)bindFloat,
    (VoidFunc)createStaticText,
    (VoidFunc)drawStaticText,
    (VoidFunc)getEngineShader,
    (VoidFunc)createDynamicText,
    (VoidFunc)setDynamicTextContents,
    (VoidFunc)drawDynamicText,
    (VoidFunc)destroyGeometry,
    (VoidFunc)destroyShader,
    (VoidFunc)destroyStaticText,
    (VoidFunc)destroyDynamicText,
    (VoidFunc)destroyBasicGrid

};

bool runGameDLL(Window& win, const char* path, HINSTANCE dll)
{
    void (*entry)(Window&, VoidFunc*);

    if(dll == NULL)
        dll = LoadLibraryA(path);

    if (dll == NULL)
        return false;

    entry = (void(*)(Window&, VoidFunc*))GetProcAddress(dll, "runGame");

    if (entry == NULL)
    {
        FreeLibrary(dll);
        return false;
    }

    entry(win, fun);

    return true;
}

#endif

// ... (rest of the code)

#endif