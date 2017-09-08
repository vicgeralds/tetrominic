CCFLAGS = $(CFLAGS) $(CPPFLAGS)
OBJS = terminal.o ansi.o acstext.o textout.o textgfx.o draw.o layout.o

terminal.a: $(OBJS)
	$(AR) rs terminal.a $(OBJS)

terminal.o: terminal.c terminal.h
	$(CC) $(CCFLAGS) -c terminal.c
ansi.o: ansi.c terminal.h
	$(CC) $(CCFLAGS) -c ansi.c
acstext.o: acstext.c terminal.h
	$(CC) $(CCFLAGS) -c acstext.c
textout.o: textout.c terminal.h
	$(CC) $(CCFLAGS) -c textout.c
textgfx.o: textgfx.c textgfx.h terminal.h
	$(CC) $(CCFLAGS) -c textgfx.c
draw.o: draw.c textgfx.h terminal.h
	$(CC) $(CCFLAGS) -c draw.c
layout.o: layout.c textgfx.h terminal.h
	$(CC) $(CCFLAGS) -c layout.c

clean:
	rm -f *.o
	rm -f *.a

.PHONY: clean
