CCFLAGS = $(CFLAGS) $(CPPFLAGS)
CONFIG_PREFIX = ..

textgfx.a: $(OBJS)
	ar rs textgfx.a $(OBJS)

textgfx.o: textgfx.c textgfx.h ../terminal/terminal.h
	$(CC) $(CCFLAGS) -I../terminal -c textgfx.c
box.o: box.c textgfx.h ../terminal/terminal.h
	$(CC) $(CCFLAGS) -I../terminal -c box.c
pattern.o: pattern.c textgfx.h
	$(CC) $(CCFLAGS) -c pattern.c
colors.o: colors.c textgfx.h
	$(CC) $(CCFLAGS) -c colors.c
ansi.o: ansi.c textgfx.h ../terminal/terminal.h
	$(CC) $(CCFLAGS) -I../terminal -c ansi.c

clean:
	rm -f *.o
	rm -f *.a

.PHONY: clean
