tetrominic: main.o game terminal
	$(CC) $(LDFLAGS) main.o game/game.a terminal/terminal.a -o tetrominic

tetrominic.js: main.o game terminal module.js
	$(CC) $(LDFLAGS) main.o game/game.a terminal/terminal.a -o tetrominic.js -s ENVIRONMENT=node --pre-js module.js

main.o: main.c terminal/terminal.h terminal/acstext.h terminal/input.h game/game.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c main.c

terminal:
	$(MAKE) -C terminal

game:
	$(MAKE) -C game

clean:
	rm -f *.o
	$(MAKE) -C terminal clean
	$(MAKE) -C game clean

.PHONY: terminal game clean
