#ifndef SAVE_LOAD_H_
#define SAVE_LOAD_H_

#include <stdbool.h>
#include "document.h"

bool LoadFile(Document *document, const char *filePath);
bool SaveFile(Document *document, const char *filePath);

#endif // SAVE_LOAD_H_