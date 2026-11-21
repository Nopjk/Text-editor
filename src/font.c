#include "font.h"

#include "rendering.h"
#include <stdio.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "../thirdparty/stb_truetype.h"

void LoadCharsToFont(SDL_Renderer *renderer, Font *font, unsigned char *data, size_t firstChar, size_t charsCount, size_t textureSize) {
    stbtt_bakedchar *cdata = malloc(charsCount * sizeof(stbtt_bakedchar));
    unsigned char *bitmapAlpha = malloc(textureSize * textureSize);
    stbtt_BakeFontBitmap(data, 0, font->pixelHeight, bitmapAlpha, textureSize, textureSize, firstChar, charsCount, cdata);

    Uint32 *bitmapRGBA = malloc(textureSize * textureSize * sizeof(Uint32));
    for (size_t i = 0; i < textureSize * textureSize; i++) {
        bitmapRGBA[i] = (0xFFFFFF00) | bitmapAlpha[i];
    }

    SDL_Texture *texture = scp(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, textureSize, textureSize));
    scc(SDL_UpdateTexture(texture, NULL, bitmapRGBA, textureSize * 4));
    scc(SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND));
    // scc(SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST));

    font->spriteSheets = realloc(font->spriteSheets, (font->spriteSheetsCount + 1) * sizeof(SDL_Texture));
    font->spriteSheets[font->spriteSheetsCount] = texture;
    font->spriteSheetsCount += 1;

    font->glyphs = realloc(font->glyphs, (font->glyphsCount + charsCount) * sizeof(Glyph));
    for (size_t i = 0; i < charsCount; i++) {
        // printf("%zu: x0: %d, x1: %d, y0: %d, y1: %d\n", i + 1, cdata[i].x0, cdata[i].x1, cdata[i].y0, cdata[i].y1);
        // printf("%zu: xoff: %f, yoff: %f, xadvance: %f\n", i + 1, cdata[i].xoff, cdata[i].yoff, cdata[i].xadvance);
        // printf("%zu: x: %d, y: %d, w: %d, h: %d\n", i + 1, cdata[i].x0, cdata[i].y0, cdata[i].x1 - cdata[i].x0, cdata[i].y1 - cdata[i].y0);
        font->glyphs[font->glyphsCount].rect.x   = cdata[i].x0;
        font->glyphs[font->glyphsCount].rect.y   = cdata[i].y0;
        font->glyphs[font->glyphsCount].rect.w   = cdata[i].x1 - cdata[i].x0;
        font->glyphs[font->glyphsCount].rect.h   = cdata[i].y1 - cdata[i].y0;
        font->glyphs[font->glyphsCount].xoff     = cdata[i].xoff;
        font->glyphs[font->glyphsCount].yoff     = cdata[i].yoff;
        font->glyphs[font->glyphsCount].xadvance = cdata[i].xadvance;
        font->glyphs[font->glyphsCount].spriteSheetIndex = font->spriteSheetsCount - 1;
        font->glyphsCount += 1;
    }

    free(cdata);
    free(bitmapAlpha);
    free(bitmapRGBA);

}

Font *LoadFont(SDL_Renderer *renderer, unsigned char *data) {
    Font *font = malloc(sizeof(Font));
    font->glyphs = malloc(sizeof(Glyph));
    font->glyphsCount = 0;
    font->pixelHeight = 64;
    font->pixelWidth  = 48;
    font->spriteSheets = malloc(sizeof(SDL_Texture));
    font->spriteSheetsCount = 0;

    size_t firstChar = ASCII_DISPLAY_LOW;
    size_t lastChar  = ASCII_DISPLAY_HIGH;
    size_t textureSize = 1024;
    LoadCharsToFont(renderer, font, data, firstChar, lastChar - firstChar + 1, textureSize);

    firstChar = CYRILLIC_DISPLAY_LOW;
    lastChar  = CYRILLIC_DISPLAY_HIGH;
    textureSize = 1024;
    LoadCharsToFont(renderer, font, data, firstChar, lastChar - firstChar + 1, textureSize);

    return font;
}

Font *LoadFontFromFile(SDL_Renderer *renderer, const char *filePath) {
    FILE *file = fopen(filePath, "rb");
    if (file == NULL) { 
        printf("Error opening file %s\n", filePath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    const size_t capacity = ftell(file);
    fseek(file, 0, SEEK_SET);
    unsigned char *data = malloc(capacity);
    fread(data, 1, capacity, file);
    fclose(file);

    Font *font = LoadFont(renderer, data);

    free(data);

    return font;
}