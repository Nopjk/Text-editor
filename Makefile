CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SOURCES = src/document.c src/cursor.c src/fileManagement.c src/la.c
INCLUDE = -I src/
LIBS = -L thirdparty/ -lm -lSDL3 -lwinmm -lgdi32 -limm32 -lole32 -loleaut32 -lversion -luuid -ladvapi32 -lsetupapi -lshell32

te:
	$(CC) $(CFLAGS) -o te te.c $(SOURCES) $(INCLUDE) $(LIBS)