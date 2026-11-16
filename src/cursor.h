#ifndef CURSOR_H_
#define CURSOR_H_

#include <stdlib.h>
#include <document.h>

typedef struct {
    size_t x, y;
    size_t lastX;
} Cursor;

void CursorSetPos(const Document *document, Cursor *cursor, ssize_t x, ssize_t y);
void CursorMove(const Document *document, Cursor *cursor, ssize_t dx, ssize_t dy);

#endif // CURSOR_H_