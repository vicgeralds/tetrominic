CCFLAGS = $(CFLAGS) $(CPPFLAGS)
OBJS = terminal.o termsize.o inputmode.o textout.o

terminal.a: $(OBJS)
	$(AR) rs terminal.a $(OBJS)

terminal.o: terminal.c terminal.h
	$(CC) $(CCFLAGS) -c terminal.c
termsize.o: termsize.c terminal.h
	$(CC) $(CCFLAGS) -c termsize.c
inputmode.o: inputmode.c terminal.h
	$(CC) $(CCFLAGS) -c inputmode.c
textout.o: textout.c terminal.h
	$(CC) $(CCFLAGS) -c textout.c

clean:
	rm -f *.o
	rm -f *.a

.PHONY: clean
