
EXEC = blockmatic
CC = gcc
CFLAGS = -Wall -Wextra -Wformat -Wconversion -Werror `sdl2-config --cflags` -std=c99 -pedantic
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf
OBJS = $(EXEC).o engine.o grid.o param.o tetri.o

$(EXEC): $(OBJS)
	if [ ! -d bin ]; then mkdir bin; fi
	$(CC) $^ -o $@ $(LDFLAGS)
	mv $@ bin/

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJS)

mrproper: clean
	rm -rf bin/$(EXEC)
	rmdir bin

# to install Blockmatic, define PATH_PREFIX in constants.h as "/usr/share/blockmatic"
install: bin/$(EXEC)
	cp bin/$(EXEC) /usr/bin
	mkdir /usr/share/$(EXEC)
	cp *.png /usr/share/$(EXEC)
	cp *.jpg /usr/share/$(EXEC)
	cp *.otf /usr/share/$(EXEC)
	cp $(EXEC)_completion /etc/bash_completion.d/$(EXEC)

uninstall:
	rm /usr/bin/$(EXEC)
	rm -R /usr/share/$(EXEC)
	rm /etc/bash_completion.d/$(EXEC)

