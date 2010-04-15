CCFLAGS = $(CFLAGS) $(CPPFLAGS)

putlatin1: putlatin1.c ../src/terminal/terminal.a
	$(CC) $(CCFLAGS) -I../src/terminal -o putlatin1 \
putlatin1.c ../src/terminal/terminal.a

putacs: putacs.c ../src/terminal/terminal.a
	$(CC) $(CCFLAGS) -I../src/terminal -o putacs \
putacs.c ../src/terminal/terminal.a

allegro-charenc: allegro-charenc.c ../src/terminal/charenc.o
	$(CC) $(CCFLAGS) -I../src/terminal -o allegro-charenc \
allegro-charenc.c ../src/terminal/charenc.o `allegro-config --libs`
