tetrominic: main.o game terminal
	$(CC) -o tetrominic main.o game/game.a terminal/terminal.a

main.o: main.c terminal/terminal.h terminal/acstext.h terminal/input.h game/game.h
	$(CC) $(CCFLAGS) -c main.c

terminal:
	$(MAKE) -C terminal

game:
	$(MAKE) -C game

.PHONY: terminal game
