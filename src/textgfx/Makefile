CCFLAGS = $(CFLAGS) $(CPPFLAGS)
OBJS = textgfx.o draw.o layout.o bitmap.o

textgfx.a: $(OBJS)
	$(AR) rs textgfx.a $(OBJS)

textgfx.o: textgfx.c textgfx.h ../terminal/terminal.h
	$(CC) $(CCFLAGS) -I../terminal -c textgfx.c
draw.o: draw.c textgfx.h ../terminal/terminal.h
	$(CC) $(CCFLAGS) -I../terminal -c draw.c
layout.o: layout.c textgfx.h ../terminal/terminal.h
	$(CC) $(CCFLAGS) -I../terminal -c layout.c
bitmap.o: bitmap.c bitmap.h
	$(CC) $(CCFLAGS) -c bitmap.c
acstext.o: acstext.c textgfx.h ../terminal/terminal.h
	$(CC) $(CCFLAGS) -I../terminal -c acstext.c

clean:
	rm -f *.o
	rm -f *.a

.PHONY: clean
