#include "ui.h"
#include <glad/glad.h>
#include "utilities.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include <fstream>
#include <iostream>

extern unsigned int currentShader;
extern Shader ag_text_shader;
extern unsigned int ag_text_shader_position_location;
extern unsigned int ag_text_shader_colour_location;

#define AG_MAX_SHARED_FONTS 4
Font sharedFonts[AG_MAX_SHARED_FONTS];
const char* sharedFontNames[AG_MAX_SHARED_FONTS];
unsigned int nSharedFonts = 0;

Font * getEngineFont(const char * name)
{
	int i = 0;
	for (; i < nSharedFonts; ++i)
	{
		if (strcmp(name, sharedFontNames[i]) == 0)
			break;
	}
	return ((i < nSharedFonts) ? &sharedFonts[i] : 0);
}

Font loadFont(const char * path)
{
	unsigned short height = 4096, width = 4096;
	unsigned char* heapAlloc = (unsigned char*)malloc(300000 + (width * width));
	unsigned char * ttf_buffer = heapAlloc;
	unsigned char * temp_bitmap = heapAlloc + 300000;

	stbtt_bakedchar* cdata = (stbtt_bakedchar*)malloc(sizeof(stbtt_bakedchar) * 96); // ASCII 32..126 is 95 glyphs
	Font font {true, 0, 0, width, (BitmapCharData*)cdata};

	FILE* f = fopen(path, "rb");
	fread(ttf_buffer, 1, 1 << 20, f);
	int retVal = stbtt_BakeFontBitmap(ttf_buffer, 0, 96.0, temp_bitmap, width, width, 32, 96, cdata); // no guarantee this fits!
	if (retVal < 0)
	{
		font.valid = false;
		return font;
	}
	// can free ttf_buffer at this point
	glGenTextures(1, &font.textureID);
	glBindTexture(GL_TEXTURE_2D, font.textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, width, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
	// can free temp_bitmap at this point
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	
	//For Testing
	/*int c = 'H' - 32;
	std::cout << "\nS0: " << cdata[c].x0 << "\nS1: " << cdata[c].x1 << "\nT0: " << cdata[c].y0 << "\nT1: " << cdata[c].y1;
	std::cout << "\nXOff: " << cdata[c].xoff << "\nYOff: " << cdata[c].yoff;
	std::cout << "\nXAdvance: " << cdata[c].xadvance;*/

	free(heapAlloc);
	fclose(f);

	IFDBG(std::cout << "Font successfully loaded!\n";)
	return font;
}

Font& loadSharedFont(const char* name, const char* path)
{
	Font* x = getEngineFont(name);
	if (x != 0)
		return *x;
	
	Font font = loadFont(path);

	if (nSharedFonts >= AG_MAX_SHARED_FONTS)
	{
		std::cerr << "Unable to share font: AG_MAX_SHARED_FONTS reached\n";
		return font;
	}

	sharedFontNames[nSharedFonts] = name;
	sharedFonts[nSharedFonts] = font;
	font.ID = nSharedFonts;
	return sharedFonts[nSharedFonts++];
}

void destroyBitmapFont(Font& font)
{
	free(font.cdata);
}

StaticText createStaticText(const char* text, Font& font)
{
	StaticText st{ 0, (unsigned int)strlen(text), &font };
	float x = 0.0f, y = 0.0f;

	TextVertex* vertices = (TextVertex*)malloc(sizeof(TextVertex) * 4 * st.length);

	for (int i = 0; i < st.length; i++)
	{
		int cv = i * 4;
		TextVertex* p = &vertices[cv];
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
		p->s = s1; p->t = t0;
		p++;

		//Top-Left
		p->x = x + xoff; p->y = y + yoff + h;
		p->s = s0; p->t = t0;
		p++;

		//Bottom-Left
		p->x = x + xoff; p->y = y + yoff;
		p->s = s0; p->t = t1;
		p++;

		//Bottom-Right
		p->x = x + xoff + w; p->y = y + yoff;
		p->s = s1; p->t = t1;

		x += font.cdata[text[i] - 32].xadvance / 1024.0f;
	}

	//Create Vertex Array, Vertex Buffer and Index Buffer
	GL(glGenVertexArrays(1, &st.vao));
	GL(glBindVertexArray(st.vao));

	unsigned int vbo, ibo;
	GL(glGenBuffers(1, &vbo));
	GL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GL(glBufferData(GL_ARRAY_BUFFER, sizeof(TextVertex) * st.length * 4, vertices, GL_STATIC_DRAW));

	GL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)0));
	GL(glEnableVertexAttribArray(0));
	GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)(sizeof(float) * 2)));
	GL(glEnableVertexAttribArray(1));

	unsigned int* indices = (unsigned int*)malloc(sizeof(unsigned int) * st.length * 6);

	int offset = 0;
	for (int i = 0; i < st.length * 4; i += 4)
	{
		indices[offset] = i + 0;
		indices[offset + 1] = i + 1;
		indices[offset + 2] = i + 2;
		indices[offset + 3] = i + 2;
		indices[offset + 4] = i + 3;
		indices[offset + 5] = i + 0;

		offset += 6;
	}

	GL(glGenBuffers(1, &ibo));
	GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * st.length * 6, indices, GL_DYNAMIC_DRAW));

	free(vertices);
	free(indices);

	return st;
}

DynamicText createDynamicText(Font& font, unsigned int maxCharacters)
{
	DynamicText tb{ 0, maxCharacters , &font};
	unsigned int n = maxCharacters;

	TextVertex* vertices = (TextVertex*)calloc(n * 4,  sizeof(TextVertex));

	unsigned int* indices = (unsigned int*)malloc(sizeof(unsigned int) * n * 6);

	int offset = 0;
	for (int i = 0; i < n * 4; i += 4)
	{
		indices[offset] = i + 0;
		indices[offset + 1] = i + 1;
		indices[offset + 2] = i + 2;
		indices[offset + 3] = i + 2;
		indices[offset + 4] = i + 3;
		indices[offset + 5] = i + 0;

		offset += 6;
	}

	unsigned int cPL[]{ 2,2 };

	//Prevents me accidentally breaking things haha! (Could be done by preprocessor tbh..)
	unsigned int stride = 0;
	for (unsigned int i = 0; i < sizeof(cPL) / 4; ++i)
		stride += cPL[i];

	tb.geom = createGeometry((float*)vertices, indices, cPL, tb.maxLength * 4, tb.maxLength * 6, 2, 4);

	return tb;
}

void setDynamicTextContents(DynamicText& dt, const char* contents)
{
	Font& font = *dt.font;
	float x = 0.0f, y = 0.0f;
	bool eos = false;
	int i;
	for (i = 0; i < dt.maxLength; i++)
	{
		if (contents[i] < 32 || contents[i] > 126)
			break;

		char c = contents[i] - 32;
		TextVertex* p = (TextVertex*)dt.geom.vertices + i * 4;
		float s0 = font.cdata[c].x0 / 4096.0f;
		float s1 = font.cdata[c].x1 / 4096.0f;
		float t0 = font.cdata[c].y0 / 4096.0f;
		float t1 = font.cdata[c].y1 / 4096.0f;

		float xoff = (font.cdata[c].xoff / 1024.0f);
		float x0 = s0 * 4.0f, x1 = s1 * 4.0f;
		float y0 = t0 * 8.0f, y1 = t1 * 8.0f;
		float w = (x1 - x0);
		float h = (y1 - y0);
		float yoff = -(font.cdata[c].yoff / 512.0f + h);

		//Top-Right
		p->x = x + xoff + w; p->y = y + yoff + h;
		p->s = s1; p->t = t0;
		p++;

		//Top-Left
		p->x = x + xoff; p->y = y + yoff + h;
		p->s = s0; p->t = t0;
		p++;

		//Bottom-Left
		p->x = x + xoff; p->y = y + yoff;
		p->s = s0; p->t = t1;
		p++;

		//Bottom-Right
		p->x = x + xoff + w; p->y = y + yoff;
		p->s = s1; p->t = t1;

		x += font.cdata[c].xadvance / 1024.0f;
	}

	dt.length = i;
	updateGeometry(dt.geom);
}

TextBox createTextBox(const char* text, float scale, Font& font, float r, float g, float b)
{
	TextBox tb {0};
	unsigned int n = tb.length = strlen(text);
	float x = -0.99f, y = 1.0f;

	tb.font = &font;
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

void drawStaticText(StaticText& text, float x, float y, float z, float r, float g, float b, float a)
{
	GL(glUniform3f(ag_text_shader_position_location, x, y, z));
	GL(glUniform4f(ag_text_shader_colour_location, r,g,b,a));

	GL(glBindTexture(GL_TEXTURE_2D, text.font->textureID));
	GL(glBindVertexArray(text.vao));
	GL(glDrawElements(GL_TRIANGLES, text.length * 6, GL_UNSIGNED_INT, (void*)0));
}

void drawDynamicText(DynamicText& text, float x, float y, float z, float r, float g, float b, float a)
{
	GL(glUniform3f(ag_text_shader_position_location, x, y, z));
	GL(glUniform4f(ag_text_shader_colour_location, r, g, b, a));

	GL(glBindTexture(GL_TEXTURE_2D, text.font->textureID));
	GL(glBindVertexArray(text.geom.vao));
	GL(glDrawElements(GL_TRIANGLES, text.length * 6, GL_UNSIGNED_INT, (void*)0));
}

void drawTextBox(TextBox& box, float x, float y)
{
	GL(glBindVertexArray(box.vao));

	GL(glBindTexture(GL_TEXTURE_2D, box.font->textureID));

	GL(unsigned int loc = glGetUniformLocation(currentShader, "trans"));

	GL(glUniform2f(loc, x, y));

	GL(glDrawElements(GL_TRIANGLES, box.length * 6, GL_UNSIGNED_INT, (void*)0));
}