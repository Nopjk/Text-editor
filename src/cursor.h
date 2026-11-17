#ifndef CURSOR_H_
#define CURSOR_H_

#include <stdlib.h>
#include <document.h>

typedef struct {
    size_t pos;
    size_t x, y;
    size_t lastX;
} Cursor;

void CursorSetPos(const Document *document, Cursor *cursor, const size_t pos);
void CursorMoveUp(const Document *document, Cursor *cursor, const size_t dy);
void CursorMoveDown(const Document *document, Cursor *cursor, const size_t dy);
void CursorMoveLeft(const Document *document, Cursor *cursor, const size_t dx);
void CursorMoveRight(const Document *document, Cursor *cursor, const size_t dx);
void CursorMove(const Document *document, Cursor *cursor, const ssize_t dx, const ssize_t dy);

#endif // CURSOR_H_