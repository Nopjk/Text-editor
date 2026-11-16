#include <string.h>
#include "mathExt.h"
#include "document.h"

#define LINE_INIT_CAPACITY 1024 
#define DOCUMENT_INIT_CAPACITY 1

void LineGrow(Line *line, size_t textSize) {
    size_t newCapacity = line->capacity;

    if (line->capacity == 0) { newCapacity = LINE_INIT_CAPACITY; }
    while (newCapacity < line->size + textSize) {
        newCapacity *= 2;
    }
    if (newCapacity != line->capacity) {
        line->chars = realloc(line->chars, newCapacity);
        line->capacity = newCapacity;
    }
}

void WriteLineSized(Line *line, const char *text, const size_t textSize, const size_t col) {
    LineGrow(line, textSize);
    
    memmove(line->chars + col + textSize,
            line->chars + col, 
            line->size - col);
    memcpy(line->chars + col, text, textSize);
    line->size += textSize;
}

void WriteLine(Line *line, const char *text, const size_t col) {
    size_t textSize = strlen(text);
    WriteLineSized(line, text, textSize, col);
}

void DeleteText(Line *line, const size_t col, size_t textSize) {
    textSize = clamp(textSize, 0, line->size - col);
    if (col < line->size && line->size > 0) {
        memmove(line->chars + col,
                line->chars + col + textSize,
                line->size - col);
        line->size -= textSize;
    }
}

void DeleteChar(Line *line, const size_t col) {
    DeleteText(line, col, 1);
}



void DocumentGrow(Document *document, size_t newLinesCount) {
    size_t newCapacity = document->capacity;

    if (document->capacity == 0) { newCapacity = DOCUMENT_INIT_CAPACITY; }
    while (newCapacity < document->linesCount + newLinesCount) {
        newCapacity *= 2;
    }
    if (newCapacity != document->capacity) {
        document->lines = realloc(document->lines, newCapacity * sizeof(Line));
        document->capacity = newCapacity;
    }
}

void AppendLine(Document *document) {
    Line line = {0};
    DocumentGrow(document, 1);

    document->lines[document->linesCount] = line;
    document->linesCount += 1;
}

void AppendLineAt(Document *document, size_t row) {
    Line line = {0};
    DocumentGrow(document, 1);

    row = clamp(row, 0, document->linesCount + 1);
    memmove(document->lines + row + 1,
            document->lines + row,
            (document->linesCount - row) * sizeof(line));
    document->lines[row] = line;
    document->linesCount += 1;
}

void RemoveLineAt(Document *document, size_t row) {
    row = clamp(row, 0, document->linesCount + 1);
    memmove(document->lines + row,
            document->lines + row + 1,
            (document->linesCount - row) * sizeof(Line));
    document->linesCount -= 1;
}