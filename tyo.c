#include <stdio.h>
#include "SDL3/SDL.h"

#include "mathExt.h"

#include "rendering.h"
#include "font.h"
#include "document.h"
#include "cursor.h"
#include "saveLoad.h"

#include "assets/icon.h"
#include "assets/baseFont.h"
#include "assets/startupMessage.h"

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

void MoveCamera(SDL_Window *window, const Font *font, const float fontScale,
                const Cursor *cursor, Vector2Float *camera) {
    int w, h;
    scc(SDL_GetWindowSizeInPixels(window, &w, &h));
    w /= font->pixelWidth  * fontScale;
    h /= font->pixelHeight * fontScale * 1.25;

    const ssize_t topDiff    =      cursor->y + camera->y     ;
    const ssize_t bottomDiff = h - (cursor->y + camera->y + 1);
    const ssize_t leftDiff   =      cursor->x + camera->x     ;
    const ssize_t rightDiff  = w - (cursor->x + camera->x + 1);
    // printf("Top: %zu, Bottom: %zu, Left: %zu, Right: %zu\n", topDiff, bottomDiff, leftDiff, rightDiff);
    // printf("Width: %d, Height: %d\n", w, h);
    
    ssize_t minDiff = 5;
    if (w <= minDiff * 2 || h <= minDiff * 2) {
        camera->x = 0.0f - cursor->x + w / 2;
        camera->y = 0.0f - cursor->y + h / 2;
    }
    else {
        if      (topDiff    < minDiff) { camera->y += minDiff - topDiff; }
        else if (bottomDiff < minDiff) { camera->y -= minDiff - bottomDiff; }

        if      (leftDiff   < minDiff) { camera->x += minDiff - leftDiff; }
        else if (rightDiff  < minDiff) { camera->x -= minDiff - rightDiff; }
    }

    if (camera->x > 0) { camera->x = 0; }
    if (camera->y > 0) { camera->y = 0; }
}

int main(int argc, char **argv) {
    SDL_Window   *window;
    SDL_Renderer *renderer;
    scc(SDL_Init(SDL_INIT_VIDEO));
    scc(SDL_CreateWindowAndRenderer("tyokst", 800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer));
    scc(SDL_StartTextInput(window));

    SetIcon(window, ICON, ICON_TEXTURE_WIDTH, ICON_TEXTURE_HEIGHT);

    Font *font = {0};
    Cursor cursor = {0};
    Document document = {0};
    Vector2Float camera = {0, 0};
    Vector2Float baseOffset = {0, 0};

    const char *filePath = "saveTest.txt";
    if (argc > 1) {
        filePath = argv[1];
        if (LoadFile(&document, filePath)) { fprintf(stdout, "Successfully loaded file %s\n", filePath); }
        else                               { fprintf(stderr, "Error loading file %s\n", filePath); return 0; }
    }
    else {
        // const char startupMessage[] = "Hello, world!";
        WriteText(&document, startupMessage, 0);
        // CursorSetPos(&document, &cursor, strlen(startupMessage));
    }


    if (argc > 2) { 
        const char *fontPath = argv[2]; 
        font = LoadFontFromFile(renderer, fontPath);
    }
    else {
        font = LoadFont(renderer, F77_Minecraft_ttf);
        // const char *fontPath = "assets/F77 Minecraft.ttf";
        // fontPath = "assets/Comic Sans MS.ttf";
        // font = LoadFontFromFile(renderer, filePath);
    }
    float fontScale = 26 / font->pixelHeight;
    float fontScaleStep = 1 / font->pixelHeight;

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
        
        // printf("X: %zu, Y: %zu, LastX: %zu, Pos: %zu\n", cursor.x, cursor.y, cursor.lastX, cursor.pos);
        MoveCamera(window, font, fontScale, &cursor, &camera);
        Vector2Float offset = Vec2Add(baseOffset, camera);
        RenderDocument(renderer, font, fontScale, 0xFFFFFFFF, &document, offset);
        RenderCursor(renderer, font, fontScale, 0xFFFFFFFF, &document, offset, &cursor);

        scc(SDL_RenderPresent(renderer));
    }

    SDL_Quit();
    return 0;
}