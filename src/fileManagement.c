#include <stdio.h>
#include "fileManagement.h"

#define ASCII_DISPLAY_LOW 32
#define ASCII_DISPLAY_HIGH 126

bool LoadFile(Document *document, const char *filePath) {
    document->linesCount = 0;

    FILE *file = fopen(filePath, "rb");
    if (file == NULL) { 
        printf("Error opening file %s\n", filePath);
        return false;
    }

    fseek(file, 0, SEEK_END);
    const size_t capacity = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = malloc(capacity);
    size_t bufferSize = fread(buffer, 1, capacity, file);
    fclose(file);

    char *str = buffer;
    size_t strSize = 0;
    size_t line = 0;
    for (size_t i = 0; i < bufferSize; i++) {
        if (buffer[i] == '\r') {
            AppendLine(document);
            WriteLineSized(&document->lines[line], str, strSize, 0);
            strSize = 0;
            line += 1;
            str = &buffer[i + 2];
            i++;
        }
        else if (buffer[i] >= ASCII_DISPLAY_LOW && buffer[i] <= ASCII_DISPLAY_HIGH){
            strSize++;
        }
        else {
            strSize++;
            printf("Unknown character (ASCII): %d\n", buffer[i]);
        }
    }
    AppendLine(document);
    WriteLineSized(&document->lines[line], str, strSize, 0);
    free(buffer);

    return true;
}

bool SaveFile(Document *document, const char *filePath) {
    FILE *file = fopen(filePath, "wb");
    if (file == NULL) { 
        printf("Error opening file %s\n", filePath);
        return false;
    }

    for (size_t i = 0; i < document->linesCount; i++) {
        const char *str = document->lines[i].chars;
        for (size_t n = 0; n < document->lines[i].size; n++) {
            putc(str[n], file);
        }
        if (i + 1 < document->linesCount) {
            putc('\r', file);
            putc('\n', file);
        }
    }
    fclose(file);

    return true;
}