CCFLAGS = $(CFLAGS) $(CPPFLAGS)
CONFIG_PREFIX = ..

terminal.a: $(OBJS)
	ar rs terminal.a $(OBJS)

terminal.o: terminal.c terminal.h
	$(CC) $(CCFLAGS) -c terminal.c
termsize.o: termsize.c $(CONFIG_PREFIX)/config/conio.h $(CONFIG_PREFIX)/config/os.h terminal.h
	$(CC) $(CCFLAGS) -I$(CONFIG_PREFIX) -c termsize.c
termtype.o: termtype.c terminal.h
	$(CC) $(CCFLAGS) -c termtype.c
inputmode.o: inputmode.c terminal.h
	$(CC) $(CCFLAGS) -c inputmode.c
acs.o: acs.c terminal.h
	$(CC) $(CCFLAGS) -c acs.c
textout.o: textout.c terminal.h
	$(CC) $(CCFLAGS) -c textout.c
charenc.o: charenc.c terminal.h
	$(CC) $(CCFLAGS) -c charenc.c
xwindow.o: xwindow.c terminal.h
	$(CC) $(CCFLAGS) $(XLIB_INC) -c xwindow.c

clean:
	rm -f *.o
	rm -f *.a

.PHONY: clean
