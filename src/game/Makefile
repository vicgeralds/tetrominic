CCFLAGS = $(CFLAGS) $(CPPFLAGS)
OBJS = game.o tetfield.o tetgrid.o tetmino.o random.o view.o

game.a: $(OBJS)
	$(AR) rs game.a $(OBJS)

game.o: game.c game.h tetmino.h tetgrid.h tetfield.h random.h view.h ../terminal/textgfx.h ../terminal/terminal.h
	$(CC) $(CCFLAGS) -c game.c
tetfield.o: tetfield.c tetmino.h tetgrid.h tetfield.h
	$(CC) $(CCFLAGS) -c tetfield.c
tetgrid.o: tetgrid.c tetmino.h tetgrid.h
	$(CC) $(CCFLAGS) -c tetgrid.c
tetmino.o: tetmino.c tetmino.h
	$(CC) $(CCFLAGS) -c tetmino.c
random.o: random.c random.h
	$(CC) $(CCFLAGS) -c random.c
view.o: view.c tetmino.h view.h ../terminal/textgfx.h
	$(CC) $(CCFLAGS) -c view.c

clean:
	rm -f *.o
	rm -f *.a

.PHONY: clean
