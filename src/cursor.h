#ifndef CURSOR_H_
#define CURSOR_H_

#include <stdlib.h>
#include <document.h>

typedef struct {
    size_t pos;
    size_t lastX;
} Cursor;

size_t CalculateX(const Document *document, size_t pos);
void CursorSetPos(const Document *document, Cursor *cursor, size_t pos);
void CursorMoveLeft(const Document *document, Cursor *cursor, const size_t delta);
void CursorMoveRight(const Document *document, Cursor *cursor, const size_t delta);
void CursorMoveUp(const Document *document, Cursor *cursor, const size_t delta);
void CursorMoveDown(const Document *document, Cursor *cursor, const size_t delta);

#endif // CURSOR_H_