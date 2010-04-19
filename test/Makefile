CCFLAGS = $(CFLAGS) $(CPPFLAGS)
terminal_dir = ../src/terminal
terminal_a = $(terminal_dir)/terminal.a
CCterminal = $(CC) $(CCFLAGS) -I$(terminal_dir)

all: putlatin1 putacs allegro-charenc xfocus

putlatin1: putlatin1.c $(terminal_a)
	$(CCterminal) -o putlatin1 putlatin1.c $(terminal_a)

putacs: putacs.c $(terminal_a)
	$(CCterminal) -o putacs putacs.c $(terminal_a)

allegro-charenc: allegro-charenc.c ../src/terminal/charenc.o
	$(CCterminal) -o allegro-charenc \
allegro-charenc.c $(terminal_dir)/charenc.o `allegro-config --libs`

xfocus: xfocus.c $(terminal_a)
	$(CCterminal) -o xfocus xfocus.c $(terminal_a) -lX11
