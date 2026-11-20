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

void RenderCursor(SDL_Renderer *renderer, const Font *font, float fontScale, Vector2Float pos) {
    SDL_FRect rect = {
        .x = pos.x,
        .y = pos.y,
        .w = font->pixelHeight * fontScale * 0.75,
        .h = font->pixelHeight * fontScale * 1.25,
    };

    scc(SDL_SetRenderDrawColor(renderer, UNHEX(0xFFFFFFFF)));
    scc(SDL_RenderFillRect(renderer, &rect));
}

float RenderChar(SDL_Renderer *renderer, const Font *font, const float fontScale, const char *text, const Vector2Float pos) {
    uint32_t c = GetUnicode(text);
    size_t index = '?' - ASCII_DISPLAY_LOW;
    
    if (ASCII_DISPLAY_HIGH >= c && c >= ASCII_DISPLAY_LOW) {
        index = c - ASCII_DISPLAY_LOW;
    }
    if (CYRILLIC_DISPLAY_HIGH >= c && c >= CYRILLIC_DISPLAY_LOW) {
        index = c - CYRILLIC_DISPLAY_LOW + (ASCII_DISPLAY_HIGH - ASCII_DISPLAY_LOW + 1);
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
        .x = pos.x + (font->pixelHeight * 0.75 - glyph.rect.w) * fontScale / 2,
        .y = pos.y + ((glyph.yoff + font->pixelHeight) * fontScale),
        .w = glyph.rect.w * fontScale,
        .h = glyph.rect.h * fontScale,
    };
    scc(SDL_RenderTexture(renderer, font->spriteSheets[glyph.spriteSheetIndex], &src, &dst));

    return font->pixelHeight * fontScale * 0.75;
}

void RenderDocument(SDL_Renderer *renderer, const Font *font, const float fontScale, const uint32_t color,
                    Document *document, Cursor *cursor, Vector2Float offset) {
    for (size_t i = 0; i < font->spriteSheetsCount; i++) {
        SetTextureColor(font->spriteSheets[i], color);
    }

    float vertPadding = 1.25;
    Vector2Float cursorPos = { -1, -1 };
    Vector2Float charPos = offset;
    charPos.y *= vertPadding;
    for (size_t i = 0; i < document->size;) {
        const char c = document->chars[i];
        const char *text = &document->chars[i];

        if (c == '\r') {
            if (i == cursor->pos) { cursorPos = charPos; }
            charPos.x = offset.x;
        }
        else if (c == '\t') {
            if (i == cursor->pos) { cursorPos = charPos; }
            charPos.x += 4 * font->pixelHeight * fontScale * 0.75;
        }
        else if (c == '\n') {
            charPos.y += font->pixelHeight * vertPadding * fontScale;
        }
        else {
            if (i == cursor->pos) { cursorPos = charPos; }
            charPos.x += RenderChar(renderer, font, fontScale, text, charPos);
        }

        i += UTF8GetCharBytesCount(text);
    }

    if (cursorPos.x == -1) { cursorPos = charPos; }
    RenderCursor(renderer, font, fontScale, cursorPos);
    if (document->chars[cursor->pos] != '\r' && document->chars[cursor->pos] != '\t' && cursor->pos != document->size) {
        for (size_t i = 0; i < font->spriteSheetsCount; i++) {
            SetTextureColor(font->spriteSheets[i], 0x000000FF);
        }
        RenderChar(renderer, font, fontScale, &document->chars[cursor->pos], cursorPos);
    }
}
