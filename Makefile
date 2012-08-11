
CC = gcc
CFLAGS = -Wall -Wextra -Wformat -Wconversion -Werror `sdl2-config --cflags` -std=c99 -pedantic
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf
EXEC = blockmatic

$(EXEC): $(EXEC).o param.o engine.o tetri.o grid.o
	$(CC) $^ -o $@ $(LDFLAGS)

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

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)

# to install Blockmatic, define PATH_PREFIX in constants.h as "/usr/share/blockmatic"
install: $(EXEC)
	cp $(EXEC) /usr/bin
	mkdir /usr/share/$(EXEC)
	cp {*.png,*.jpg,*.otf} /usr/share/$(EXEC)
	cp $(EXEC)_completion /etc/bash_completion.d/$(EXEC)

uninstall:
	rm /usr/bin/$(EXEC)
	rm -R /usr/share/$(EXEC)
	rm /etc/bash_completion.d/$(EXEC)
