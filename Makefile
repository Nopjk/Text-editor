CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SOURCES = te.c line.c cursor.c fileManagement.c la.c
INCLUDE = -I src/
LIBS = -L thirdparty/ -lm -lSDL3 -lwinmm -lgdi32 -limm32 -lole32 -loleaut32 -lversion -luuid -ladvapi32 -lsetupapi -lshell32

te:
	$(CC) $(CFLAGS) -o te $(SOURCES) $(INCLUDE) $(LIBS)