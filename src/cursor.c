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

void CursorSetPos(const Document *document, Cursor *cursor, size_t pos) {
    if (pos > document->size) { pos = document->size; }

    cursor->pos = pos;
    cursor->lastX = CalculateX(document, cursor->pos);
}

void CursorMoveLeft(const Document *document, Cursor *cursor, const size_t delta) {
    size_t remaining = delta;
    while (1) {
        if      (remaining == 0) { break; }
        else if (cursor->pos == 0) { break; }
        else if (document->chars[cursor->pos - 1] == '\n') { break; }

        cursor->pos -= UTF8GetPrevCharBytesCount(document, cursor->pos);
        remaining -= 1;
    }
    cursor->lastX = CalculateX(document, cursor->pos);
}

void CursorMoveRight(const Document *document, Cursor *cursor, const size_t delta) {
    size_t remaining = delta;
    while (1) {
        if      (remaining == 0) { break; }
        else if (document->chars[cursor->pos] == '\r') { break; }
        else if (cursor->pos == document->size) { break; }

        cursor->pos += UTF8GetCharBytesCount(&document->chars[cursor->pos]);
        remaining -= 1;
    }
    cursor->lastX = CalculateX(document, cursor->pos);
}

void CursorMoveUp(const Document *document, Cursor *cursor, const size_t delta) {
    size_t remaining = delta + 1;
    while (1) {
        if (cursor->pos == 0) { break; }
        else if (document->chars[cursor->pos - 1] == '\n') { remaining -= 1; }
        if (remaining == 0) { break; }

        cursor->pos -= UTF8GetPrevCharBytesCount(document, cursor->pos);
    }

    size_t saveLastX = cursor->lastX;
    CursorMoveRight(document, cursor, cursor->lastX);
    cursor->lastX = saveLastX;
}

void CursorMoveDown(const Document *document, Cursor *cursor, const size_t delta) {
    size_t saveCursorPos = cursor->pos;
    size_t remaining = delta;
    if (cursor->pos != 0 && document->chars[cursor->pos - 1] == '\n') { remaining += 1; }
    while (1) {
        if (cursor->pos == document->size) { 
            cursor->pos = saveCursorPos;
            return;
        }
        else if (document->chars[cursor->pos - 1] == '\n') {
            remaining -= 1;
        }
        if (remaining == 0) { break; }

        cursor->pos += UTF8GetCharBytesCount(&document->chars[cursor->pos]);
    }

    size_t saveLastX = cursor->lastX;
    CursorMoveRight(document, cursor, cursor->lastX);
    cursor->lastX = saveLastX;
}