#include <stdio.h>
#include <stdlib.h>
#include "SDL3/SDL.h"

#include "mathExt.h"

#include "rendering.h"
#include "font.h"
#include "document.h"
#include "cursor.h"
#include "saveLoad.h"

#include "assets/icon.h"

void Delete(Document *document, Cursor *cursor) {
    if (document->chars[cursor->pos] != '\r') {
        DeleteChar(document, cursor->pos);
    }
}

void Backspace(Document *document, Cursor *cursor) {
    if (cursor->pos == 0) { return; }

    if (document->chars[cursor->pos - 1] == '\n') {
        CursorSetPos(document, cursor, cursor->pos - 2);
        DeleteText(document, cursor->pos, 2);
    }
    else {
        size_t size = UTF8GetPrevCharBytesCount(document, cursor->pos);
        CursorMoveLeft(document, cursor, 1);
        DeleteText(document, cursor->pos, size);
    }
}

void Return(Document *document, Cursor *cursor) {
    WriteText(document, "\r\n", cursor->pos);
    CursorSetPos(document, cursor, cursor->pos + 2);
}


int main(int argc, char **argv) {
    SDL_Window   *window;
    SDL_Renderer *renderer;
    scc(SDL_Init(SDL_INIT_VIDEO));
    scc(SDL_CreateWindowAndRenderer("tyokst", 800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer));
    scc(SDL_StartTextInput(window));

    SetIcon(window, ICON, ICON_TEXTURE_WIDTH, ICON_TEXTURE_HEIGHT);
    const char *fontPath = "assets/F77 Minecraft.ttf";
    // fontPath = "assets/Comic Sans MS.ttf";
    Font *font = LoadFont(renderer, fontPath);
    float fontScale = 26 / font->pixelHeight;
    float fontScaleStep = 1 / font->pixelHeight;

    Cursor cursor = {0};
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
        CursorSetPos(&document, &cursor, strlen(startupMessage));
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
                        case SDLK_UP:    { CursorMoveUp   (&document, &cursor, 1); } break;
                        case SDLK_DOWN:  { CursorMoveDown (&document, &cursor, 1); } break;
                        case SDLK_LEFT:  { CursorMoveLeft (&document, &cursor, 1); } break;
                        case SDLK_RIGHT: { CursorMoveRight(&document, &cursor, 1); } break;
                        case SDLK_TAB: {
                            // WriteText(&document, "\t", cursor.pos);
                            // CursorMoveRight(&document, &cursor, 1);
                            WriteText(&document, "    ", cursor.pos);
                            CursorMoveRight(&document, &cursor, 4);
                        } break;

                        case SDLK_DELETE:    { Delete   (&document, &cursor); } break;
                        case SDLK_BACKSPACE: { Backspace(&document, &cursor); } break;
                        case SDLK_RETURN:    { Return   (&document, &cursor); } break;

                        case SDLK_EQUALS: {
                            if (event.key.mod & SDL_KMOD_CTRL) {
                                fontScale += fontScaleStep;
                            }
                        } break;
                        case SDLK_MINUS: {
                            if (event.key.mod & SDL_KMOD_CTRL) {
                                if (fontScale - fontScaleStep >= (1 / font->pixelHeight)) {
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
                    CursorMoveRight(&document, &cursor, 1);
                } break;
            }
        }
        scc(SDL_SetRenderDrawColor(renderer, UNHEX(0x112233FF)));
        scc(SDL_RenderClear(renderer));

        // printf("LastX: %zu, Pos: %zu\n", cursor.lastX, cursor.pos);
        Vector2Float offset = { 100, 100 };
        RenderDocument(renderer, font, fontScale, 0xFFFFFFFF, &document, &cursor, offset);

        scc(SDL_RenderPresent(renderer));
    }

    SDL_Quit();
    return 0;
}