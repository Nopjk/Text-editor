#ifndef FILE_MANAGEMENT_H_
#define FILE_MANAGEMENT_H_

#include <stdbool.h>
#include "document.h"

bool LoadFile(Document *document, const char *filePath);
bool SaveFile(Document *document, const char *filePath);

#endif // FILE_MANAGEMENT_H_