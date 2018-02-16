CCFLAGS = $(CFLAGS) $(CPPFLAGS)
OBJS = terminal.o ansi.o textout.o acstext.o draw.o layout.o

terminal.a: $(OBJS)
	$(AR) rs terminal.a $(OBJS)

terminal.o: terminal.c terminal.h
	$(CC) $(CCFLAGS) -c terminal.c
ansi.o: ansi.c terminal.h
	$(CC) $(CCFLAGS) -c ansi.c
textout.o: textout.c terminal.h
	$(CC) $(CCFLAGS) -c textout.c
acstext.o: acstext.c acstext.h terminal.h
	$(CC) $(CCFLAGS) -c acstext.c
draw.o: draw.c textgfx.h terminal.h
	$(CC) $(CCFLAGS) -c draw.c
layout.o: layout.c terminal.h
	$(CC) $(CCFLAGS) -c layout.c

clean:
	rm -f *.o
	rm -f *.a

.PHONY: clean
