CCFLAGS = $(CFLAGS) $(CPPFLAGS)
OBJS = tetfield.o tetmino.o randseq.o view.o

game.a: $(OBJS)
	$(AR) rs game.a $(OBJS)

tetfield.o: tetfield.c tetmino.h tetfield.h
	$(CC) $(CCFLAGS) -c tetfield.c
tetmino.o: tetmino.c tetmino.h
	$(CC) $(CCFLAGS) -c tetmino.c
randseq.o: randseq.c randseq.h
	$(CC) $(CCFLAGS) -c randseq.c
view.o: view.c tetmino.h view.h
	$(CC) $(CCFLAGS) -c view.c

clean:
	rm -f *.o
	rm -f *.a

.PHONY: clean
