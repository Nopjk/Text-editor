#include "mathExt.h"
#include "cursor.h"

void CursorSetPos(const Document *document, Cursor *cursor, const size_t pos) {
    cursor->pos = 0;
    cursor->x = 0;
    cursor->y = 0;

    while (cursor->pos < pos && cursor->pos < document->size) {
        const char c = document->chars[cursor->pos];
        if (c == '\r') {
            cursor->x = 0;
        }
        else if (c == '\n') {
            cursor->y += 1;
        }
        else {
            cursor->x += 1;
        }
        cursor->pos += 1;
    }
    cursor->lastX = cursor->x;
}

void CursorMoveUp(const Document *document, Cursor *cursor, const size_t dy) {
    if (cursor->y == 0) {
        return;
    }
    const size_t savePos = cursor->pos;
    const size_t saveY = cursor->y;
    const size_t saveLastX = cursor->lastX;
    size_t remaining = clamp(dy, 0, cursor->y);

    if (cursor->x != 0) {
        remaining += 1;
        cursor->y += 1;
    }

    while (true) {
        cursor->pos -= 1;
        if (document->chars[cursor->pos - 1] == '\n') {
            remaining -= 1;
            cursor->y -= 1;
        }
        else if (cursor->pos == 0) {
            remaining -= 1;
            cursor->y -= 1;
            break;
        }
        if (remaining == 0) {
            break;
        }
    }

    if (cursor->y < saveY) {
        cursor->x = 0;
        CursorMoveRight(document, cursor, saveLastX);
        cursor->lastX = saveLastX;
    }
    else {
        cursor->pos = savePos;
    }
}

void CursorMoveDown(const Document *document, Cursor *cursor, const size_t dy) {
    if (cursor->pos == document->size) {
        return;
    }
    const size_t savePos = cursor->pos;
    const size_t saveY = cursor->y;
    const size_t saveLastX = cursor->lastX;
    size_t remaining = dy;

    while (true) {
        cursor->pos += 1;
        if (document->chars[cursor->pos - 1] == '\n') {
            remaining -= 1;
            cursor->y += 1;
        }
        if (cursor->pos == document->size) {
            break;
        }
        if (remaining == 0) {
            break;
        }
    }

    if (cursor->y > saveY) {
        cursor->x = 0;
        CursorMoveRight(document, cursor, saveLastX);
        cursor->lastX = saveLastX;
    }
    else {
        cursor->pos = savePos;
    }
}

void CursorMoveLeft(const Document *document, Cursor *cursor, const size_t dx) {
    size_t remaining = dx;
    while (true) {
        if (remaining == 0) {
            break;
        }
        if (cursor->x == 0) {
            break;
        }
        if (document->chars[cursor->pos - 1] == '\n') {
            break;
        }
        cursor->pos -= 1;
        cursor->x -= 1;
        cursor->lastX = cursor->x;
        remaining -= 1;
    }
}

void CursorMoveRight(const Document *document, Cursor *cursor, const size_t dx) {
    size_t remaining = dx;
    while (true) {
        if (remaining == 0) {
            break;
        }
        if (document->chars[cursor->pos] == '\r') {
            break;
        }
        if (cursor->pos == document->size) {
            break;
        }
        cursor->pos += 1;
        cursor->x += 1;
        cursor->lastX = cursor->x;
        remaining -= 1;
    }
}

void CursorMove(const Document *document, Cursor *cursor, const ssize_t dx, const ssize_t dy) {
    if (dx > 0) {
        CursorMoveRight(document, cursor, dx);
    }
    else if (dx < 0) {
        CursorMoveLeft(document, cursor, -dx);
    }

    if (dy > 0) {
        CursorMoveDown(document, cursor, dy);
    }
    else if (dy < 0) {
        CursorMoveUp(document, cursor, -dy);
    }
}