#include <stdio.h>
#include "saveLoad.h"

#define ASCII_DISPLAY_LOW 32
#define ASCII_DISPLAY_HIGH 126

bool LoadFile(Document *document, const char *filePath) {
    document->size = 0;

    FILE *file = fopen(filePath, "rb");
    if (file == NULL) { 
        printf("Error opening file %s\n", filePath);
        return false;
    }

    fseek(file, 0, SEEK_END);
    const size_t capacity = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = malloc(capacity);
    const size_t bufferSize = fread(buffer, 1, capacity, file);
    fclose(file);

    WriteTextSized(document, buffer, bufferSize, 0);
    free(buffer);

    return true;
}

bool SaveFile(Document *document, const char *filePath) {
    FILE *file = fopen(filePath, "wb");
    if (file == NULL) { 
        printf("Error opening file %s\n", filePath);
        return false;
    }

    fwrite(document->chars, sizeof(char), document->size, file);
    fclose(file);

    return true;
}