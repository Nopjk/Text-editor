#include <stdio.h>
#include <stdlib.h>
#include "SDL3/SDL.h"

#include "mathExt.h"

#include "document.h"
#include "cursor.h"
#include "saveLoad.h"

#include "assets/font.h"

#define ASCII_DISPLAY_LOW 32
#define ASCII_DISPLAY_HIGH 126

#define UNHEX(color)               \
        (color >> (8 * 3)) & 0xFF, \
        (color >> (8 * 2)) & 0xFF, \
        (color >> (8 * 1)) & 0xFF, \
        (color >> (8 * 0)) & 0xFF

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

typedef struct {
    SDL_Texture *spritesheet;
    SDL_FRect glyph_table[ASCII_DISPLAY_HIGH - ASCII_DISPLAY_LOW + 1];
} Font;

Font LoadFont(SDL_Renderer *renderer) {
    Font font = {0};
    SDL_Surface *surface = scp(SDL_CreateSurfaceFrom(FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT, SDL_PIXELFORMAT_RGBA8888, FONT, FONT_TEXTURE_WIDTH*4));
    scc(SDL_SetSurfaceColorKey(surface, true, 0x000000FF));
    font.spritesheet = scp(SDL_CreateTextureFromSurface(renderer, surface));
    scc(SDL_SetTextureScaleMode(font.spritesheet, SDL_SCALEMODE_NEAREST));
    SDL_DestroySurface(surface);

    for (size_t ascii = ASCII_DISPLAY_LOW; ascii <= ASCII_DISPLAY_HIGH; ascii++) {
        const size_t index = ascii - ASCII_DISPLAY_LOW;
        const size_t col = index % FONT_TEXTURE_COLS;
        const size_t row = index / FONT_TEXTURE_COLS;

        font.glyph_table[index] = (SDL_FRect) {
            .x = col * FONT_CHAR_WIDTH,
            .y = row * FONT_CHAR_HEIGHT,
            .w = FONT_CHAR_WIDTH,
            .h = FONT_CHAR_HEIGHT,
        };
    }

    return font;
}

void RenderChar(SDL_Renderer *renderer, const Font *font, const float fontScale, const char c, const Vector2Float pos) {
    SDL_FRect dst = {
        .x = pos.x,
        .y = pos.y,
        .w = FONT_CHAR_WIDTH * fontScale,
        .h = FONT_CHAR_HEIGHT * fontScale,
    };
    size_t index = c - ASCII_DISPLAY_LOW;

    if (c < ASCII_DISPLAY_LOW || c > ASCII_DISPLAY_HIGH) {
        index = '?' - ASCII_DISPLAY_LOW;
    }
    scc(SDL_RenderTexture(renderer, font->spritesheet, &font->glyph_table[index], &dst));
}

void SetTextureColor(SDL_Texture *texture, uint32_t color) {
    scc(SDL_SetTextureColorMod(texture,
                           (color >> (8 * 3)) & 0xFF,
                           (color >> (8 * 2)) & 0xFF,
                           (color >> (8 * 1)) & 0xFF));
    scc(SDL_SetTextureAlphaMod(texture, 
                           (color >> (8 * 0)) & 0xFF));
}

void RenderDocument(SDL_Renderer *renderer, const Font *font, const float fontScale, const uint32_t color, Document *document, Vector2Float offset) {
    SetTextureColor(font->spritesheet, color);

    const Vector2Float charSize = { FONT_CHAR_WIDTH * fontScale, FONT_CHAR_HEIGHT * fontScale };
    Vector2Float charPos = { 0, 0 };
    for (size_t i = 0; i < document->size; i++) {
        const char c = document->chars[i];
        if (c == '\r') {
            charPos.x = 0;
        }
        else if (c == '\n') {
            charPos.y += 1;
        }
        else {
            const Vector2Float pos = Vec2Add(Vec2Mult(charPos, charSize), offset);
            RenderChar(renderer, font, fontScale, c, pos);
            charPos.x += 1;
        }
    }
}

void RenderCursor(SDL_Renderer *renderer, const Font *font, float fontScale, Document *document, Cursor cursor, Vector2Float offset) {
    Vector2Float pos = vec2f(cursor.x * FONT_CHAR_WIDTH * fontScale,
                             cursor.y * FONT_CHAR_HEIGHT * fontScale);
    pos = Vec2Add(pos, offset);

    SDL_FRect rect = {
        .x = pos.x,
        .y = pos.y,
        .w = FONT_CHAR_WIDTH * fontScale,
        .h = FONT_CHAR_HEIGHT * fontScale,
    };

    scc(SDL_SetRenderDrawColor(renderer, UNHEX(0xFFFFFFFF)));
    scc(SDL_RenderFillRect(renderer, &rect));

    if (document->chars[cursor.pos] != '\r' && cursor.pos != document->size) {
        SetTextureColor(font->spritesheet, 0x000000FF);
        RenderChar(renderer, font, fontScale, document->chars[cursor.pos], pos);
    }
}

float fontScaleStep = 1;
float fontScale = 3;

void MoveCamera(SDL_Window *window, Vector2Float *cameraPos, const Cursor *cursor) {
    int w, h;
    SDL_GetWindowSizeInPixels(window, &w, &h);
    size_t cursorPosX = cursor->x * FONT_CHAR_WIDTH * fontScale;
    ssize_t cursorPosY = cursor->y * FONT_CHAR_HEIGHT * fontScale;
    size_t linesPadding = 5;
    ssize_t minDiff = FONT_CHAR_HEIGHT * fontScale * linesPadding;

    size_t windowBottomBorderPos = cameraPos->y + h;
    ssize_t botDiff = windowBottomBorderPos - (cursorPosY + FONT_CHAR_HEIGHT * fontScale);
    if (botDiff < minDiff) {
        cameraPos->y += minDiff - botDiff;
    }

    size_t windowTopBorderPos = cameraPos->y;
    ssize_t topDiff = cursorPosY - windowTopBorderPos;
    if (topDiff < minDiff) {
        cameraPos->y -= minDiff - topDiff;
        if (cameraPos->y < 0) {
            cameraPos->y = 0;
        }
    }

    size_t charsPadding = 10;
    minDiff = FONT_CHAR_WIDTH * fontScale * charsPadding;
    size_t windowRightBorderPos = cameraPos->x + w;
    ssize_t rightDiff = windowRightBorderPos - (cursorPosX + FONT_CHAR_WIDTH * fontScale);
    if (rightDiff < minDiff) {
        cameraPos->x += minDiff - rightDiff;
    }

    size_t windowLeftBorderPos = cameraPos->x;
    ssize_t leftDiff = cursorPosX - windowLeftBorderPos;
    if (leftDiff < minDiff) {
        cameraPos->x -= minDiff - leftDiff;
        if (cameraPos->x < 0) {
            cameraPos->x = 0;
        }
    }
}

void Backspace(Document *document, Cursor *cursor) {
    if (cursor->x > 0) {
        DeleteChar(document, cursor->pos - 1);
        CursorMoveLeft(document, cursor, 1);
    }
    else if (cursor->y > 0) {
        DeleteText(document, cursor->pos - 2, 2);
        CursorSetPos(document, cursor, cursor->pos - 2);
    }
}

void Return(Document *document, Cursor *cursor) {
    WriteText(document, "\r\n", cursor->pos);
    CursorSetPos(document, cursor, cursor->pos + 2);
}

int main(int argc, char **argv) {
    scc(SDL_Init(SDL_INIT_VIDEO));
    SDL_Window   *window;
    SDL_Renderer *renderer;
    scc(SDL_CreateWindowAndRenderer("te", 800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer));
    const Font font = LoadFont(renderer);
    scc(SDL_StartTextInput(window));

    Cursor cursor = {0};
    Vector2Float cameraPos = vec2f(0, 0);

    Document document = {0};
    const char *filePath = "saveTest.txt";
    if (argc > 1) {
        filePath = argv[1];

        if (LoadFile(&document, filePath)) { fprintf(stdout, "Successfully loaded file %s\n", filePath); }
        else                               { fprintf(stderr, "Error loading file %s\n", filePath); return 0; }
    }
    else {
        const char startupMessage[] = "Hello, world!";
        WriteText(&document, startupMessage, 0);
        CursorMoveRight(&document, &cursor, strlen(startupMessage));
    }

    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT: { 
                    quit = true; 
                } break;

                case SDL_EVENT_KEY_DOWN: {
                    switch (event.key.key) {
                        case SDLK_UP:    { CursorMoveUp(&document, &cursor, 1); } break;
                        case SDLK_DOWN:  { CursorMoveDown(&document, &cursor, 1); } break;
                        case SDLK_LEFT:  { CursorMoveLeft(&document, &cursor, 1); } break;
                        case SDLK_RIGHT: { CursorMoveRight(&document, &cursor, 1); } break;
                        case SDLK_TAB: {
                            WriteText(&document, "\t", cursor.pos);
                            CursorMoveRight(&document, &cursor, 1);
                        } break;

                        case SDLK_DELETE: {
                            if (document.chars[cursor.pos] != '\r') {
                                DeleteChar(&document, cursor.pos);
                            }
                        } break;
                        case SDLK_BACKSPACE: {
                            Backspace(&document, &cursor);
                        } break;
                        case SDLK_RETURN: {
                            Return(&document, &cursor);
                        } break;

                        case SDLK_EQUALS: {
                            if (event.key.mod & SDL_KMOD_CTRL) {
                                fontScale += fontScaleStep;
                            }
                        } break;
                        case SDLK_MINUS: {
                            if (event.key.mod & SDL_KMOD_CTRL) {
                                if (fontScale >= 1 + fontScaleStep) {
                                    fontScale -= fontScaleStep;
                                }
                            }
                        } break;

                        case SDLK_S: {
                            if (event.key.mod & SDL_KMOD_CTRL) {
                                if (SaveFile(&document, filePath)) { fprintf(stdout, "Successfully saved file %s\n", filePath); }
                                else                               { fprintf(stderr, "Error saving file %s\n", filePath); }
                            }
                        } break;
                    }
                } break;

                case SDL_EVENT_TEXT_INPUT: {
                    WriteText(&document, event.text.text, cursor.pos);
                    CursorMoveRight(&document, &cursor, strlen(event.text.text));
                } break;
            }
        }

        scc(SDL_SetRenderDrawColor(renderer, UNHEX(0x181818FF)));
        scc(SDL_RenderClear(renderer));

        MoveCamera(window, &cameraPos, &cursor);

        // printf("X: %zu, LastX: %zu, Y: %zu, Pos: %zu\n", cursor.x, cursor.lastX, cursor.y, cursor.pos);
        Vector2Float offset = { -cameraPos.x, -cameraPos.y };
        RenderDocument(renderer, &font, fontScale, 0xFFFFFFFF, &document, offset );
        RenderCursor(renderer, &font, fontScale, &document, cursor, offset );

        scc(SDL_RenderPresent(renderer));
    }

    SDL_Quit();
    return 0;
}