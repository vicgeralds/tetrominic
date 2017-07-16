CCFLAGS = $(CFLAGS) $(CPPFLAGS)
CONFIG_PREFIX = ..

textgfx.a: $(OBJS)
	$(AR) rs textgfx.a $(OBJS)

textgfx.o: textgfx.c textgfx.h ../terminal/terminal.h
	$(CC) $(CCFLAGS) -I../terminal -c textgfx.c
ansi.o: ansi.c textgfx.h ../terminal/terminal.h
	$(CC) $(CCFLAGS) -I../terminal -c ansi.c
draw.o: draw.c textgfx.h
	$(CC) $(CCFLAGS) -c draw.c
layout.o: layout.c textgfx.h
	$(CC) $(CCFLAGS) -c layout.c
bitmap.o: bitmap.c bitmap.h
	$(CC) $(CCFLAGS) -c bitmap.c
acstext.o: acstext.c textgfx.h
	$(CC) $(CCFLAGS) -c acstext.c

clean:
	rm -f *.o
	rm -f *.a

.PHONY: clean
