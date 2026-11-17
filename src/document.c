#include <string.h>
#include "mathExt.h"
#include "document.h"

#define DOCUMENT_INIT_CAPACITY 1024 

void DocumentGrow(Document *document, size_t textSize) {
    size_t newCapacity = document->capacity;

    if (document->capacity == 0) { newCapacity = DOCUMENT_INIT_CAPACITY; }
    while (newCapacity < document->size + textSize) {
        newCapacity *= 2;
    }
    if (newCapacity != document->capacity) {
        document->chars = realloc(document->chars, newCapacity);
        document->capacity = newCapacity;
    }
}

void WriteTextSized(Document *document, const char *text, const size_t textSize, const size_t pos) {
    DocumentGrow(document, textSize);
    
    memmove(document->chars + pos + textSize,
            document->chars + pos, 
            document->size - pos);
    memcpy(document->chars + pos, text, textSize);
    document->size += textSize;
}

void WriteText(Document *document, const char *text, const size_t pos) {
    size_t textSize = strlen(text);
    WriteTextSized(document, text, textSize, pos);
}

void DeleteText(Document *document, const size_t pos, const size_t textSize) {
    if (textSize > document->size - pos) {
        // textSize = document->size - pos;
        document->size -= textSize;
        return;
    }

    memmove(document->chars + pos,
            document->chars + pos + textSize,
            document->size - pos);
    document->size -= textSize;
}

void DeleteChar(Document *document, const size_t pos) {
    DeleteText(document, pos, 1);
}