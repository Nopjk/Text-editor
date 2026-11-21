#ifndef RENDERING_H_
#define RENDERING_H_

#include <SDL3/SDL.h>
#include "mathExt.h"
#include "font.h"
#include "document.h"
#include "cursor.h"

void scc(const bool code);
void *scp(void *ptr);

void SetTextureColor(SDL_Texture *texture, uint32_t color);

void SetIcon(SDL_Window *window, uint32_t *bitmap, size_t w, size_t h);

void RenderCursor(SDL_Renderer *renderer, const Font *font, float fontScale, const uint32_t color,
                  const Document *document, const Vector2Float offset, const Cursor *cursor);
void RenderChar(SDL_Renderer *renderer, const Font *font, const float fontScale, const char *text, const Vector2Float pos);
void RenderDocument(SDL_Renderer *renderer, const Font *font, const float fontScale, const uint32_t color,
                    const Document *document, const Vector2Float offset);

#endif // RENDERING_H_