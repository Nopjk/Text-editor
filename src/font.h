#ifndef FONT_H_
#define FONT_H_

#include <SDL3/SDL.h>

#define ASCII_DISPLAY_LOW 0x0020
#define ASCII_DISPLAY_HIGH 0x007E
#define CYRILLIC_DISPLAY_LOW 0x0400
#define CYRILLIC_DISPLAY_HIGH 0x045F

#define UNHEX(color)               \
        (color >> (8 * 3)) & 0xFF, \
        (color >> (8 * 2)) & 0xFF, \
        (color >> (8 * 1)) & 0xFF, \
        (color >> (8 * 0)) & 0xFF

typedef struct {
    SDL_FRect rect;
    float xoff, yoff, xadvance;
    unsigned char spriteSheetIndex;
} Glyph;

typedef struct {
    Glyph *glyphs;
    size_t glyphsCount;
    float pixelHeight;
    float pixelWidth;
    SDL_Texture **spriteSheets;
    size_t spriteSheetsCount;
} Font;

Font *LoadFont(SDL_Renderer *renderer, unsigned char *data);
Font *LoadFontFromFile(SDL_Renderer *renderer, const char *filePath);

#endif // FONT_H_