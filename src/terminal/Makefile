CCFLAGS = $(CFLAGS) $(CPPFLAGS)

OBJS = terminal.o termsize.o termtype.o inputmode.o \
       acs.o textout.o charenc.o \
       xwindow.o

terminal.a: $(OBJS)
	ar rs terminal.a $(OBJS)

terminal.o: terminal.c terminal.h
	$(CC) $(CCFLAGS) -c terminal.c

termsize.o: termsize.c terminal.h ../config.h
	$(CC) $(CCFLAGS) -I.. -c termsize.c

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
	rm -f $(OBJS)

.PHONY: clean
