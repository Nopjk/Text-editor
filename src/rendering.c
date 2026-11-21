#include "rendering.h"

#include <stdio.h>

void scc(const bool code) {
    if (code == false) {
        fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
        exit(1);
    }
}

void *scp(void *ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
        exit(1);
    }

    return ptr;
}

void SetTextureColor(SDL_Texture *texture, uint32_t color) {
    scc(SDL_SetTextureColorMod(texture,
                           (color >> (8 * 3)) & 0xFF,
                           (color >> (8 * 2)) & 0xFF,
                           (color >> (8 * 1)) & 0xFF));
    scc(SDL_SetTextureAlphaMod(texture, 
                           (color >> (8 * 0)) & 0xFF));
}

void SetIcon(SDL_Window *window, uint32_t *bitmap, size_t w, size_t h) {
    SDL_Surface *icon = scp(SDL_CreateSurfaceFrom(w,
                                                  h,
                                                  SDL_PIXELFORMAT_RGBA8888,
                                                  bitmap,
                                                  w * 4));
    icon = SDL_ScaleSurface(icon, 128, 128, SDL_SCALEMODE_NEAREST);
    scc(SDL_SetSurfaceColorKey(icon, true, 0x000000FF));
    scc(SDL_SetWindowIcon(window, icon));
}

void RenderCursor(SDL_Renderer *renderer, const Font *font, float fontScale, const uint32_t color,
                  const Document *document, const Vector2Float offset, const Cursor *cursor) {
    Vector2Float pos = {
        .x = cursor->x + offset.x,
        .y = cursor->y + offset.y,
    };

    SDL_FRect rect = {
        .x = pos.x * font->pixelWidth  * fontScale,
        .y = pos.y * font->pixelHeight * fontScale * 1.25,
        .w = font->pixelWidth  * fontScale,
        .h = font->pixelHeight * fontScale * 1.25,
    };

    scc(SDL_SetRenderDrawColor(renderer, UNHEX(color)));
    scc(SDL_RenderFillRect(renderer, &rect));
    
    char c = document->chars[cursor->pos];
    if (c == '\r' || c == '\t' || cursor->pos == document->size) { return; }
    for (size_t i = 0; i < font->spriteSheetsCount; i++) {
        SetTextureColor(font->spriteSheets[i], 0x000000FF);
    }
    RenderChar(renderer, font, fontScale, &document->chars[cursor->pos], pos);
}

void RenderChar(SDL_Renderer *renderer, const Font *font, const float fontScale, const char *text, const Vector2Float pos) {
    uint32_t c = GetUnicode(text);
    size_t index = '?' - ASCII_DISPLAY_LOW;
    
    if (ASCII_DISPLAY_HIGH >= c && c >= ASCII_DISPLAY_LOW) {
        index = c - ASCII_DISPLAY_LOW;
    }
    if (CYRILLIC_DISPLAY_HIGH >= c && c >= CYRILLIC_DISPLAY_LOW) {
        index = c - CYRILLIC_DISPLAY_LOW +
                (ASCII_DISPLAY_HIGH - ASCII_DISPLAY_LOW + 1);
    }

    Glyph glyph = font->glyphs[index];
    SDL_FRect src = glyph.rect;

    // SDL_FRect dst = {
    //     .x = pos.x + glyph.xoff,
    //     .y = pos.y + ((glyph.yoff + font->pixelHeight) * fontScale),
    //     .w = glyph.rect.w * fontScale,
    //     .h = glyph.rect.h * fontScale,
    // };

    SDL_FRect dst = {
        .x = (pos.x * font->pixelWidth  * fontScale) + (font->pixelWidth - glyph.rect.w) * fontScale / 2,
        .y = (pos.y * font->pixelHeight * fontScale) * 1.25 + ((glyph.yoff + font->pixelHeight) * fontScale),
        .w = glyph.rect.w * fontScale,
        .h = glyph.rect.h * fontScale,
    };
    scc(SDL_RenderTexture(renderer, font->spriteSheets[glyph.spriteSheetIndex], &src, &dst));
}

void RenderDocument(SDL_Renderer *renderer, const Font *font, const float fontScale, const uint32_t color,
                    const Document *document, const Vector2Float offset) {
    for (size_t i = 0; i < font->spriteSheetsCount; i++) {
        SetTextureColor(font->spriteSheets[i], color);
    }

    Vector2Float charPos = offset;
    for (size_t i = 0; i < document->size;) {
        const char c = document->chars[i];
        const char *text = &document->chars[i];

        if (c == '\r') {
            charPos.x = offset.x;
        }
        else if (c == '\t') {
            charPos.x += 4;
        }
        else if (c == '\n') {
            charPos.y += 1;
        }
        else {
            RenderChar(renderer, font, fontScale, text, charPos);
            charPos.x += 1;
        }

        i += UTF8GetCharBytesCount(text);
    }
}
