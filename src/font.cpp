// #define STB_TRUETYPE_IMPLEMENTATION
// #include "stb_truetype.h"

#define FONT_WIDTH_LIMIT 1024
#define FONT_HEIGHT_LIMIT 1024

struct Font {
	stbtt_fontinfo info;
};

void initFont(void *ttfData, Font *font);
void generateText(Font *font, const char *text, int size);

unsigned char *currentTextBitmap = NULL; // This is a 1 channel bitmap!!!
unsigned char *currentTextBitmap4bpp = NULL;

void initFont(void *ttfData, Font *font) {
	int ret = stbtt_InitFont(&font->info, (unsigned char *)ttfData, 0);
	if (!ret) {
		printf("Faied to parse font\n");
		return;
	}

	if (!currentTextBitmap) {
		currentTextBitmap = (unsigned char *)malloc(FONT_WIDTH_LIMIT * FONT_HEIGHT_LIMIT);
		currentTextBitmap4bpp = (unsigned char *)malloc(FONT_WIDTH_LIMIT * FONT_HEIGHT_LIMIT * 4);
		memset(currentTextBitmap4bpp, 0, FONT_WIDTH_LIMIT * FONT_HEIGHT_LIMIT * 4);
	}
}

void generateText(Font *font, const char *text, int size) {
	float scale = stbtt_ScaleForPixelHeight(&font->info, size);

	int ascent, descent, lineGap;
	stbtt_GetFontVMetrics(&font->info, &ascent, &descent, &lineGap);

	ascent *= scale;
	descent *= scale;

	int x = 0;
	int lineNum = 0;
	int textLen = strlen(text);
	memset(currentTextBitmap, 0, FONT_WIDTH_LIMIT * FONT_HEIGHT_LIMIT);

	int maxX = 0;
	for (int i = 0; i < textLen; i++) {
		if (text[i] == '\n') {
			lineNum++;
			x = 0;
			continue;
		}

		int c_x1, c_y1, c_x2, c_y2;
		stbtt_GetCodepointBitmapBox(&font->info, text[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

		int y = ascent + c_y1 + lineGap*lineNum;

		int byteOffset = x + y*FONT_WIDTH_LIMIT;
		stbtt_MakeCodepointBitmap(&font->info, currentTextBitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, FONT_WIDTH_LIMIT, scale, scale, text[i]);

		int ax;
		stbtt_GetCodepointHMetrics(&font->info, text[i], &ax, 0);
		x += ax*scale;

		int kern;
		kern = stbtt_GetCodepointKernAdvance(&font->info, text[i], text[i+1]);
		x += kern*scale;

		if (x > maxX) maxX = x;
	}
	int maxY = ascent + lineGap*(lineNum+1);

	memset(currentTextBitmap4bpp, 0, FONT_HEIGHT_LIMIT * FONT_WIDTH_LIMIT * 4);
	for (int ybyte = 0; ybyte < maxY; ybyte++) {
		for (int xbyte = 0; xbyte < maxX; xbyte++) {
			unsigned char byte = currentTextBitmap[ybyte * FONT_WIDTH_LIMIT + xbyte];
			int xdestbyte = xbyte;
			int ydestbyte = FONT_HEIGHT_LIMIT - ybyte - 1;
			currentTextBitmap4bpp[(ydestbyte * FONT_WIDTH_LIMIT + xdestbyte) * 4 + 0] = byte;
			currentTextBitmap4bpp[(ydestbyte * FONT_WIDTH_LIMIT + xdestbyte) * 4 + 1] = byte;
			currentTextBitmap4bpp[(ydestbyte * FONT_WIDTH_LIMIT + xdestbyte) * 4 + 2] = byte;
			currentTextBitmap4bpp[(ydestbyte * FONT_WIDTH_LIMIT + xdestbyte) * 4 + 3] = byte;
		}
	}
}
