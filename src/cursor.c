#include "cursor.h"

size_t CalculateX(const Document *document, size_t pos) {
    size_t x = 0;
    while (1) {
        if      (pos == 0) { break; }
        else if (document->chars[pos - 1] == '\n') { break; }

        pos -= UTF8GetPrevCharBytesCount(document, pos);
        x += 1;
    }

    return x;
}

void CursorSetPos(const Document *document, Cursor *cursor, const size_t pos) {
    cursor->pos = 0;
    cursor->x = 0;
    cursor->y = 0;
    cursor->lastX = 0;

    size_t x = 0;
    size_t y = 0;
    size_t i = 0;
    while (i < document->size && i < pos) {
        const char c = document->chars[i];
        const char *text = &document->chars[i];

        if (c == '\r') {
            x = 0;
        }
        else if (c == '\t') {
            x += 4;
        }
        else if (c == '\n') {
            y += 1;
        }
        else {
            x += 1;
        }

        i += UTF8GetCharBytesCount(text);
    }

    cursor->pos = i;
    cursor->x = x;
    cursor->y = y;
    cursor->lastX = x;
}

void CursorMoveLeft(const Document *document, Cursor *cursor, const size_t delta) {
    size_t remaining = delta;
    while (1) {
        if      (remaining == 0) { break; }
        else if (cursor->pos == 0) { break; }
        else if (document->chars[cursor->pos - 1] == '\n') { break; }

        cursor->pos -= UTF8GetPrevCharBytesCount(document, cursor->pos);
        cursor->x -= 1;
        remaining -= 1;
    }
    // cursor->lastX = CalculateX(document, cursor->pos);
    cursor->lastX = cursor->x;
}

void CursorMoveRight(const Document *document, Cursor *cursor, const size_t delta) {
    size_t remaining = delta;
    while (1) {
        if      (remaining == 0) { break; }
        else if (document->chars[cursor->pos] == '\r') { break; }
        else if (cursor->pos == document->size) { break; }

        cursor->pos += UTF8GetCharBytesCount(&document->chars[cursor->pos]);
        cursor->x += 1;
        remaining -= 1;
    }
    // cursor->lastX = CalculateX(document, cursor->pos);
    cursor->lastX = cursor->x;
}

void CursorMoveUp(const Document *document, Cursor *cursor, const size_t delta) {
    size_t remaining = delta + 1;
    cursor->y += 1;
    while (1) {
        if (cursor->pos == 0) { 
            cursor->y -= 1;
            break; 
        }
        else if (document->chars[cursor->pos - 1] == '\n') {
            cursor->y -= 1;
            remaining -= 1;
        }
        if (remaining == 0) { break; }

        cursor->pos -= UTF8GetPrevCharBytesCount(document, cursor->pos);
    }

    size_t saveLastX = cursor->lastX;
    cursor->x = 0;
    CursorMoveRight(document, cursor, cursor->lastX);
    cursor->lastX = saveLastX;
}

void CursorMoveDown(const Document *document, Cursor *cursor, const size_t delta) {
    size_t saveCursorPos = cursor->pos;
    size_t saveCursorY   = cursor->y;
    size_t remaining = delta;
    if (cursor->pos != 0 && document->chars[cursor->pos - 1] == '\n') {
        cursor->y -= 1;
        remaining += 1;
    }
    while (1) {
        if (cursor->pos == document->size) {
            cursor->pos = saveCursorPos;
            cursor->y   = saveCursorY;
            return;
        }
        else if (document->chars[cursor->pos - 1] == '\n') {
            cursor->y += 1;
            remaining -= 1;
        }
        if (remaining == 0) { break; }

        cursor->pos += UTF8GetCharBytesCount(&document->chars[cursor->pos]);
    }

    size_t saveLastX = cursor->lastX;
    cursor->x = 0;
    CursorMoveRight(document, cursor, cursor->lastX);
    cursor->lastX = saveLastX;
}