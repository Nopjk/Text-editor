CC = gcc
CFLAGS = -Wall -Wextra -std=c99
CWINDOWSFLAGS = -mwindows
SOURCES = src/rendering.c src/font.c src/document.c src/cursor.c src/saveLoad.c src/mathExt.c
INCLUDE = -I src/
LIBS = -L thirdparty/ -lm -lSDL3 -lwinmm -lgdi32 -limm32 -lole32 -loleaut32 -lversion -luuid -ladvapi32 -lsetupapi -lshell32

tyo:
	$(CC) $(CFLAGS) -o tyo tyo.c $(SOURCES) $(INCLUDE) $(LIBS)
# 	$(CC) $(CFLAGS) -o tyo tyo.c $(SOURCES) $(INCLUDE) $(LIBS) $(CWINDOWSFLAGS)