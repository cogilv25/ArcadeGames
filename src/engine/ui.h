#ifndef AG_UI_H
#define AG_UI_H
#include "render.h"

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

struct Font
{
    bool valid;
    unsigned int ID;
    unsigned int textureID;
    unsigned int width;
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
    Font* font;
    Geometry geom;
};

struct TextVertex
{
	float x, y;
	float s, t;
};

struct StaticText
{
	unsigned int vao;
	unsigned int length;
	Font* font;
};

struct DynamicText
{
	unsigned int length;
	unsigned int maxLength;
    Font* font;
	Geometry geom;
};



#ifndef AG_GAME_DLL

Font loadFont(const char* path);
Font& loadSharedFont(const char* name, const char* path);
void destroyBitmapFont(Font&);

StaticText createStaticText(const char* text, Font&);
DynamicText createDynamicText(Font&, unsigned int maxCharacters);
TextBox createTextBox(const char* text, float scale, Font&, float r, float g, float b);

void setDynamicTextContents(DynamicText& dt, const char* contents);

void drawStaticText(StaticText& text, float x, float y, float z, float r, float g, float b, float a);
void drawDynamicText(DynamicText& text, float x, float y, float z, float r, float g, float b, float a);
void drawTextBox(TextBox& box, float x, float y);

#endif
#endif