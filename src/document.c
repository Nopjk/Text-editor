#include "document.h"

#include <stdio.h>
#include <string.h>

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
        document->size -= document->size - pos;
        return;
    }

    memmove(document->chars + pos,
            document->chars + pos + textSize,
            document->size - pos);
    document->size -= textSize;
}

void DeleteChar(Document *document, const size_t pos) {
    size_t size = UTF8GetCharBytesCount(&document->chars[pos]);
    DeleteText(document, pos, size);
}



size_t UTF8GetCharBytesCount(const char *text) {
    if      ((text[0] & 0b10000000) == 0b00000000) { return 1; }
    else if ((text[0] & 0b11100000) == 0b11000000) { return 2; }
    else if ((text[0] & 0b11110000) == 0b11100000) { return 3; }
    else if ((text[0] & 0b11111000) == 0b11110000) { return 4; }
    else {
        printf("Error calculating char length\n");
        return 0;
    }
}

size_t UTF8GetPrevCharBytesCount(const Document *document, const size_t pos) {
    if (pos == 0) {
        return UTF8GetCharBytesCount(&document->chars[pos]);
    }
    size_t n = pos - 1;
    if (n >= document->size) { n = document->size - 2; }
    while ((document->chars[n] & 0b11000000) == 0b10000000) { n -= 1; }
    return UTF8GetCharBytesCount(&document->chars[n]);
}

size_t GetUnicode(const char *text) {
    size_t result = 0;
    switch (UTF8GetCharBytesCount(text)) {
        case 1: {
            result += text[0];
            return result;
        }

        case 2: {
            result += (text[0] & 0b00011111);
            result <<= 6;
            result += (text[1] & 0b00111111);
            return result;
        }

        case 3: {
            result += (text[0] & 0b00001111);
            result <<= 6;
            result += (text[1] & 0b00111111);
            result <<= 6;
            result += (text[2] & 0b00111111);
            return result;
        }

        case 4: {
            result += (text[0] & 0b00000111);
            result <<= 6;
            result += (text[1] & 0b00111111);
            result <<= 6;
            result += (text[2] & 0b00111111);
            result <<= 6;
            result += (text[3] & 0b00111111);
            return result;
        }
    }

    return 0;
}