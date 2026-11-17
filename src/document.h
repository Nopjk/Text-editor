#ifndef LINE_H_
#define LINE_H_

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    char *chars;
    size_t size;
    size_t capacity;
} Document;

// typedef struct {
//     Line *lines;
//     size_t linesCount;
//     size_t capacity;
// } Tabs;

void DocumentGrow(Document *document, size_t textSize);
void WriteTextSized(Document *document, const char *text, const size_t textSize, const size_t pos);
void WriteText(Document *document, const char *text, const size_t pos);
void DeleteText(Document *document, const size_t pos, const size_t textSize);
void DeleteChar(Document *document, const size_t pos);

// void TabGrow(tab *tab, size_t newLinesCount);
// void AppendLine(tab *tab);
// void AppendLineAt(tab *tab, size_t row);
// void RemoveLineAt(tab *tab, size_t row);

#endif // LINE_H_