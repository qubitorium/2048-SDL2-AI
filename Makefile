CC = gcc -g
CFLAGS = -std=c99 -Iinclude
LDFLAGS = -lSDL2 -lSDL2_ttf

SRC = src/main.c src/game.c src/graphics.c src/menu.c src/save.c src/scores.c src/global.c
OBJ = $(SRC:.c=.o)
EXE = 2048

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(EXE)

run:
	./$(EXE)
