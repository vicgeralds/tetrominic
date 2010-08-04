src_dir = ../src
terminal_dir = $(src_dir)/terminal
terminal_a = $(terminal_dir)/terminal.a
textgfx_a = $(src_dir)/textgfx/textgfx.a

CCFLAGS = $(CFLAGS) $(CPPFLAGS)
CPPFLAGS += -I$(terminal_dir)

all: putlatin1 putacs allegro-charenc xfocus boxes drawtext

putlatin1: putlatin1.c $(terminal_a)
	$(CC) $(CCFLAGS) -o putlatin1 putlatin1.c $(terminal_a)

putacs: putacs.c $(terminal_a)
	$(CC) $(CCFLAGS) -o putacs putacs.c $(terminal_a)

allegro-charenc: allegro-charenc.c $(terminal_dir)/charenc.o
	$(CC) $(CCFLAGS) -o allegro-charenc \
allegro-charenc.c $(terminal_dir)/charenc.o `allegro-config --libs`

xfocus: xfocus.c $(terminal_a)
	$(CC) $(CCFLAGS) -o xfocus xfocus.c $(terminal_a) -lX11

boxes: boxes.c $(textgfx_a) $(terminal_a)
	$(CC) $(CCFLAGS) -I$(src_dir)/textgfx -o boxes boxes.c $(textgfx_a) $(terminal_a)

drawtext: drawtext.c $(textgfx_a) $(terminal_a)
	$(CC) $(CCFLAGS) -I$(src_dir)/textgfx -o drawtext drawtext.c $(textgfx_a) $(terminal_a)
