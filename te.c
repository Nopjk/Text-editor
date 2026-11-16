#include <stdio.h>
#include <stdlib.h>
#include "SDL3/SDL.h"
#include "la.h"

#include "src/document.h"
#include "src/cursor.h"
#include "src/fileManagement.h"

#include "assets/font.h"

#define WIDTH  800
#define HEIGHT 600

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

void SetTextureColor(SDL_Texture *texture, const uint32_t color) {
    scc(SDL_SetTextureColorMod(texture,
                           (color >> (8 * 3)) & 0xFF,
                           (color >> (8 * 2)) & 0xFF,
                           (color >> (8 * 1)) & 0xFF));
    scc(SDL_SetTextureAlphaMod(texture, 
                           (color >> (8 * 0)) & 0xFF));
}

void RenderChar(SDL_Renderer *renderer, const Font *font, const float fontScale, const char c, const Vector2Float pos) {
    SDL_FRect dst = {
        .x = pos.x,
        .y = pos.y,
        .w = FONT_CHAR_WIDTH * fontScale,
        .h = FONT_CHAR_HEIGHT * fontScale,
    };
    size_t index = c - ASCII_DISPLAY_LOW;

    // assert(c >= ASCII_DISPLAY_LOW);
    // assert(c <= ASCII_DISPLAY_HIGH);
    if (c < ASCII_DISPLAY_LOW || c > ASCII_DISPLAY_HIGH) {
        // printf("Unknown character: %d\n", c);
        index = '?' - ASCII_DISPLAY_LOW;
    }
    scc(SDL_RenderTexture(renderer, font->spritesheet, &font->glyph_table[index], &dst));
}

void RenderTextSized(SDL_Renderer *renderer, const Font *font, const float fontScale, const uint32_t color, const char *text, const size_t size, Vector2Float pos) {
    SetTextureColor(font->spritesheet, color);

    for (size_t i = 0; i < size; i++) {
        RenderChar(renderer, font, fontScale, text[i], pos);
        pos.x += FONT_CHAR_WIDTH * fontScale;
    }
}

void RenderText(SDL_Renderer *renderer, const Font *font, const float fontScale, const uint32_t color, const char *text, Vector2Float pos) {
    RenderTextSized(renderer, font, fontScale, color, text, strlen(text), pos);
}

void RenderDocument(SDL_Renderer *renderer, const Font *font, const float fontScale, const uint32_t color, Document *document, Vector2Float cameraPos) {
    for (size_t i = 0; i < document->linesCount; i++) {
        Vector2Float linePos = vec2f(0, FONT_CHAR_HEIGHT * fontScale* i);
        linePos = Vec2Sub(linePos, cameraPos);
        RenderTextSized(renderer, font, fontScale, color, document->lines[i].chars, document->lines[i].size, linePos);
    }
}

void RenderCursor(SDL_Renderer *renderer, const Font *font, float fontScale, Document *document, Cursor cursor, Vector2Float cameraPos) {
    Vector2Float pos = vec2f(cursor.x * FONT_CHAR_WIDTH * fontScale,
                             cursor.y * FONT_CHAR_HEIGHT * fontScale);
    pos = Vec2Sub(pos, cameraPos);

    SDL_FRect rect = {
        .x = pos.x,
        .y = pos.y,
        .w = FONT_CHAR_WIDTH * fontScale,
        .h = FONT_CHAR_HEIGHT * fontScale,
    };

    scc(SDL_SetRenderDrawColor(renderer, UNHEX(0xFFFFFFFF)));
    scc(SDL_RenderFillRect(renderer, &rect));

    if (cursor.x < document->lines[cursor.y].size) {
        SetTextureColor(font->spritesheet, 0x000000FF);
        RenderChar(renderer, font, fontScale, document->lines[cursor.y].chars[cursor.x], pos);
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
    if (botDiff < minDiff)
    {
        cameraPos->y += minDiff - botDiff;
    }

    size_t windowTopBorderPos = cameraPos->y;
    ssize_t topDiff = cursorPosY - windowTopBorderPos;
    if (topDiff < minDiff)
    {
        cameraPos->y -= minDiff - topDiff;
        if (cameraPos->y < 0)
        {
            cameraPos->y = 0;
        }
    }

    size_t windowRightBorderPos = cameraPos->x + w;
    ssize_t rightDiff = windowRightBorderPos - (cursorPosX + FONT_CHAR_WIDTH * fontScale);
    if (rightDiff < minDiff)
    {
        cameraPos->x += minDiff - rightDiff;
    }

    size_t windowLeftBorderPos = cameraPos->x;
    ssize_t leftDiff = cursorPosX - windowLeftBorderPos;
    if (leftDiff < minDiff)
    {
        cameraPos->x -= minDiff - leftDiff;
        if (cameraPos->x < 0)
        {
            cameraPos->x = 0;
        }
    }
}

int main(int argc, char **argv) {
    scc(SDL_Init(SDL_INIT_VIDEO));
    SDL_Window   *window;
    SDL_Renderer *renderer;
    scc(SDL_CreateWindowAndRenderer("te", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer));
    const Font font = LoadFont(renderer);
    scc(SDL_StartTextInput(window));

    Cursor cursor = {0};
    Vector2Float cameraPos = vec2f(0, 0);

    Document document = {0};
    const char *filePath = "saveTest.txt";
    if (argc > 1) {
        filePath = argv[1];

        if (LoadFile(&document, filePath)) {
            printf("Successfully loaded file %s\n", filePath);
        }
        else {
            // printf("Error loading file %s\n", filePath);
            fprintf(stderr, "Error loading file %s\n", filePath);
            return 0;
        }
    }
    else {
        AppendLine(&document);
        const char startupMessage[] = "Hello, world!";
        WriteLine(&document.lines[0], startupMessage, 0);
        CursorMove(&document, &cursor, strlen(startupMessage), 0);
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
                        case SDLK_UP:    { CursorMove(&document, &cursor, 0, -1); } break;
                        case SDLK_DOWN:  { CursorMove(&document, &cursor, 0, 1);  } break;
                        case SDLK_LEFT:  { CursorMove(&document, &cursor, -1, 0); } break;
                        case SDLK_RIGHT: { CursorMove(&document, &cursor, 1, 0);  } break;
                        case SDLK_TAB: {
                            WriteLine(&document.lines[cursor.y], "\t", cursor.x);
                            CursorMove(&document, &cursor, 1, 0);
                        } break;

                        case SDLK_DELETE: {
                            DeleteChar(&document.lines[cursor.y], cursor.x);
                        } break;
                        case SDLK_BACKSPACE: {
                            if (cursor.x > 0) {
                                DeleteChar(&document.lines[cursor.y], cursor.x - 1);
                                CursorMove(&document, &cursor, -1, 0);
                            }
                            else if (cursor.y > 0) {
                                CursorSetPos(&document, &cursor, document.lines[cursor.y - 1].size, cursor.y - 1);
                                const char *text = document.lines[cursor.y + 1].chars;
                                const size_t textSize = document.lines[cursor.y + 1].size;
                                WriteLineSized(&document.lines[cursor.y],
                                               text,
                                               textSize,
                                               cursor.x);
                                RemoveLineAt(&document, cursor.y + 1);
                            }
                        } break;
                        case SDLK_RETURN: {
                            AppendLineAt(&document, cursor.y + 1);
                            const char *text = document.lines[cursor.y].chars + cursor.x;
                            const size_t textSize = document.lines[cursor.y].size - cursor.x;
                            WriteLineSized(&document.lines[cursor.y + 1],
                                           text,
                                           textSize,
                                           0);
                            DeleteText(&document.lines[cursor.y], cursor.x, textSize);
                            CursorSetPos(&document, &cursor, 0, cursor.y + 1);
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
                                if (SaveFile(&document, filePath)) {
                                    printf("Successfully saved file %s\n", filePath);
                                }
                                else {
                                    // printf("Error saving file %s\n", filePath);
                                    fprintf(stderr, "Error saving file %s\n", filePath);
                                    // return 0;
                                }
                            }
                        } break;
                    }
                } break;

                case SDL_EVENT_TEXT_INPUT: {
                    WriteLine(&document.lines[cursor.y], event.text.text, cursor.x);
                    CursorMove(&document, &cursor, strlen(event.text.text), 0);
                } break;
            }
        }

        scc(SDL_SetRenderDrawColor(renderer, UNHEX(0x181818FF)));
        scc(SDL_RenderClear(renderer));

        MoveCamera(window, &cameraPos, &cursor);

        RenderDocument(renderer, &font, fontScale, 0xFFFFFFFF, &document, cameraPos);
        RenderCursor(renderer, &font, fontScale, &document, cursor, cameraPos);

        scc(SDL_RenderPresent(renderer));
    }

    SDL_Quit();
	return 0;
}