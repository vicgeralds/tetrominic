CCFLAGS = $(CFLAGS) $(CPPFLAGS)
OBJS = tetfield.o tetmino.o random.o view.o

game.a: $(OBJS)
	$(AR) rs game.a $(OBJS)

tetfield.o: tetfield.c tetmino.h tetfield.h
	$(CC) $(CCFLAGS) -c tetfield.c
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
