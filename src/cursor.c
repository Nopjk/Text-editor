#include "la.h"
#include "cursor.h"

void CursorSetPos(const Document *document, Cursor *cursor, ssize_t x, ssize_t y) {
    if (x < 0) { x = 0; }
    if (y < 0) { y = 0; }

    cursor->x = x;
    cursor->y = y;
    cursor->y = clamp(cursor->y, 0, document->linesCount - 1);
    cursor->x = clamp(cursor->x, 0, document->lines[cursor->y].size);
    cursor->lastX = cursor->x;
}

void CursorMove(const Document *document, Cursor *cursor, ssize_t dx, ssize_t dy) {
    if ((ssize_t)cursor->x < -dx) { dx = -cursor->x; }
    if ((ssize_t)cursor->y < -dy) { dy = -cursor->y; }

    if (dx == 0) {
        cursor->x = cursor->lastX;
    }
    cursor->x += dx;
    cursor->y += dy;
    cursor->y = clamp(cursor->y, 0, document->linesCount - 1);
    cursor->x = clamp(cursor->x, 0, document->lines[cursor->y].size);
    if (dx != 0) {
        cursor->lastX = cursor->x;
    }
}