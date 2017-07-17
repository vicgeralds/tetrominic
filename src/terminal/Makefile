CCFLAGS = $(CFLAGS) $(CPPFLAGS)
CONFIG_PREFIX = ..

terminal.a: $(OBJS)
	$(AR) rs terminal.a $(OBJS)

terminal.o: terminal.c terminal.h
	$(CC) $(CCFLAGS) -c terminal.c
termsize.o: termsize.c $(CONFIG_PREFIX)/config/conio.h $(CONFIG_PREFIX)/config/os.h terminal.h
	$(CC) $(CCFLAGS) -I$(CONFIG_PREFIX) -c termsize.c
inputmode.o: inputmode.c terminal.h
	$(CC) $(CCFLAGS) -c inputmode.c
textout.o: textout.c terminal.h
	$(CC) $(CCFLAGS) -c textout.c

clean:
	rm -f *.o
	rm -f *.a

.PHONY: clean
