#ifndef LINE_H_
#define LINE_H_

#include <stdlib.h>

typedef struct {
    char *chars;
    size_t size;
    size_t capacity;
} Document;

void DocumentGrow(Document *document, size_t textSize);
void WriteTextSized(Document *document, const char *text, const size_t textSize, const size_t pos);
void WriteText(Document *document, const char *text, const size_t pos);
void DeleteText(Document *document, const size_t pos, const size_t textSize);
void DeleteChar(Document *document, const size_t pos);

size_t UTF8GetCharBytesCount(const char *text);
size_t UTF8GetPrevCharBytesCount(const Document *document, const size_t pos);
size_t GetUnicode(const char *text);

#endif // LINE_H_