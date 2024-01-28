#ifndef AG_UI_H
#define AG_UI_H

struct BitmapCharData;

struct UIVertex;

struct Font;

struct TextBox;

Font loadFont(const char* path);
Font& loadSharedFont(const char* name, const char* path);
void destroyBitmapFont(Font&);

TextBox createTextBox(const char* text, float scale, Font&, float r, float g, float b);
void drawTextBox(TextBox& box, float x, float y);

#endif