
CC = gcc
CFLAGS = -Wall -Wextra -Wformat -Wconversion -Werror `sdl2-config --cflags` -std=c99 -pedantic
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf
EXEC = Blockmatic

$(EXEC): $(EXEC).o param.o engine.o tetri.o grid.o
	$(CC) $(LDFLAGS) $^ -o $@

$(EXEC).o: $(EXEC).c
	$(CC) $(CFLAGS) -c $^ -o $@

param.o: param.c
	$(CC) $(CFLAGS) -c $^ -o $@

engine.o: engine.c
	$(CC) $(CFLAGS) -c $^ -o $@

tetri.o: tetri.c
	$(CC) $(CFLAGS) -c $^ -o $@

grid.o: grid.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean :
	rm -rf *.o

mrproper : clean
	rm -rf $(EXEC)
