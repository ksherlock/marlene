PROG	= fctelnet
OBJS	= o/fctelnet.a o/vt100.a o/telnet.a o/ansi.a o/chars.a o/marinetti.a o/display.a

OPTIMIZE ?= 79

CFLAGS = -w-1 -O $(OPTIMIZE)
CC = occ --gno

$(PROG): $(OBJS)
	$(RM) o/ansi.root
	$(RM) o/chars.root
	$(CC) $(OBJS) -o $@


fctelnet.o:	fctelnet.c marinetti.h
vt100.o:	vt100.c CLAGS+=-r
ansi.o:		ansi.asm
chars.o:	chars.asm
marinetti.o:	marinetti.c CLAGS+=-r
telnet.o:	telnet.c CLAGS+=-r

o/%.a : %.c
	$(CC) -c $(CFLAGS) -o $@ $^

o/%.a : %.asm
	$(CC) -c $(CFLAGS) -o $@ $^


clean:
	$(RM) -f *.o *.root *.a *.r
clobber: clean
	$(RM) -f $(PROG)
