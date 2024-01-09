#include "ui.h"
#include "binary_interface.h"
#include <glad/glad.h>
#include "utilities.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include <fstream>
#include <iostream>

extern unsigned int currentShader;

BitmapFont loadFontBitmap(const char * path)
{
	unsigned short height = 4096, width = 4096;
	unsigned char* heapAlloc = (unsigned char*)malloc(300000 + (height * width));
	unsigned char * ttf_buffer = heapAlloc;
	unsigned char * temp_bitmap = heapAlloc + 300000;

	stbtt_bakedchar* cdata = (stbtt_bakedchar*)malloc(sizeof(stbtt_bakedchar) * 96); // ASCII 32..126 is 95 glyphs
	BitmapFont font {0, width, height, (BitmapCharData*)cdata};


	fread(ttf_buffer, 1, 1 << 20, fopen(path, "rb"));
	stbtt_BakeFontBitmap(ttf_buffer, 0, 96.0, temp_bitmap, width, height, 32, 96, cdata); // no guarantee this fits!
	// can free ttf_buffer at this point
	glGenTextures(1, &font.ID);
	glBindTexture(GL_TEXTURE_2D, font.ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
	// can free temp_bitmap at this point
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	//For Testing
	/*int c = 'H' - 32;
	std::cout << "\nS0: " << cdata[c].x0 << "\nS1: " << cdata[c].x1 << "\nT0: " << cdata[c].y0 << "\nT1: " << cdata[c].y1;
	std::cout << "\nXOff: " << cdata[c].xoff << "\nYOff: " << cdata[c].yoff;
	std::cout << "\nXAdvance: " << cdata[c].xadvance;*/

	free(heapAlloc);
	return font;
}

void destroyBitmapFont(BitmapFont& font)
{
	free(font.cdata);
}

TextBox createTextBox(const char* text, float scale, BitmapFont& font, float r, float g, float b)
{
	TextBox tb {0};
	unsigned int n = tb.length = strlen(text);
	float x = -0.99f, y = 1.0f;

	tb.vertices = new UIVertex[n * 4];

	for (int i = 0; i < n; i++)
	{
		int cv = i * 4;
		UIVertex* p = &tb.vertices[cv];
		float s0 = font.cdata[text[i] - 32].x0 / 4096.0f;
		float s1 = font.cdata[text[i] - 32].x1 / 4096.0f;
		float t0 = font.cdata[text[i] - 32].y0 / 4096.0f;
		float t1 = font.cdata[text[i] - 32].y1 / 4096.0f;
		float xoff = (font.cdata[text[i] - 32].xoff / 1024.0f);
		float x0 = s0 * 4.0f, x1 = s1 * 4.0f;
		float y0 = t0 * 8.0f, y1 = t1 * 8.0f;
		float w = (x1 - x0);
		float h = (y1 - y0);
		float yoff = -(font.cdata[text[i] - 32].yoff / 512.0f + h);

		//Top-Right
		p->x = x + xoff + w; p->y = y + yoff + h;
		p->r = r; p->g = g; p->b = b;
		p->s = s1; p->t = t0;
		p++;

		//Top-Left
		p->x = x + xoff; p->y = y + yoff + h;
		p->r = r; p->g = g; p->b = b;
		p->s = s0; p->t = t0;
		p++;

		//Bottom-Left
		p->x = x + xoff; p->y = y + yoff;
		p->r = r; p->g = g; p->b = b;
		p->s = s0; p->t = t1;
		p++;

		//Bottom-Right
		p->x = x + xoff + w; p->y = y + yoff;
		p->r = r; p->g = g; p->b = b;
		p->s = s1; p->t = t1;

		x += font.cdata[text[i] - 32].xadvance/1024.0f;
	}

	//Create Vertex Array, Vertex Buffer and Index Buffer
	GL(glGenVertexArrays(1, &tb.vao));
	GL(glBindVertexArray(tb.vao));

	GL(glGenBuffers(1, &tb.vbo));
	GL(glBindBuffer(GL_ARRAY_BUFFER, tb.vbo));
	GL(glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertex) * n * 4, tb.vertices, GL_DYNAMIC_DRAW));

	GL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)0));
	GL(glEnableVertexAttribArray(0));
	GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)(sizeof(float)*2)));
	GL(glEnableVertexAttribArray(1));
	GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)(sizeof(float) * 5)));
	GL(glEnableVertexAttribArray(2));

	unsigned int* indices_data = new unsigned int[n * 6];

	int offset = 0;
	for (int i = 0; i < n * 4; i += 4)
	{
		indices_data[offset] = i + 0;
		indices_data[offset + 1] = i + 1;
		indices_data[offset + 2] = i + 2;
		indices_data[offset + 3] = i + 2;
		indices_data[offset + 4] = i + 3;
		indices_data[offset + 5] = i + 0;

		offset += 6;
	}

	GL(glGenBuffers(1, &tb.ibo));
	GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tb.ibo));
	GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * n * 6, indices_data, GL_DYNAMIC_DRAW));

	delete[] indices_data;
	delete tb.vertices;

	return tb;
}

void drawTextBox(TextBox& box, float x, float y)
{
	GL(glBindVertexArray(box.vao));

	GL(unsigned int loc = glGetUniformLocation(currentShader, "trans"));

	GL(glUniform2f(loc, x, y));

	GL(glDrawElements(GL_TRIANGLES, box.length * 6, GL_UNSIGNED_INT, (void*)0));
}