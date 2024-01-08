#ifndef AG_UI_H
#define AG_UI_H

struct BitmapCharData;

struct UIVertex;

struct BitmapFont;

struct TextBox;

BitmapFont loadFontBitmap(const char* path);
void destroyBitmapFont(BitmapFont&);

TextBox createTextBox(const char* text, float scale, BitmapFont&, float r, float g, float b);
void drawTextBox(TextBox& box, float x, float y);

#endif