#ifndef LINE_H_
#define LINE_H_

#include <stdlib.h>

typedef struct {
    char *chars;
    size_t size;
    size_t capacity;
} Line;

typedef struct {
    Line *lines;
    size_t linesCount;
    size_t capacity;
} Document;

void LineGrow(Line *line, size_t n);
void WriteLineSized(Line *line, const char *text, const size_t textSize, const size_t col);
void WriteLine(Line *line, const char *text, const size_t col);
void DeleteText(Line *line, const size_t col, const size_t n);
void DeleteChar(Line *line, const size_t col);

void DocumentGrow(Document *document, size_t n);
void AppendLine(Document *document);
void AppendLineAt(Document *document, size_t row);
void RemoveLineAt(Document *document, size_t row);

#endif // LINE_H_