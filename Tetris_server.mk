SOURCE = src/main_server.c
LIBRARY = -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_net
BUILD = build/tetris_server

all:
	gcc -I deps/Include -L deps/lib -o $(BUILD) $(SOURCE) $(LIBRARY)