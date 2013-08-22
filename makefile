PROG	= fctelnet
OBJS	= fctelnet.o vt100.o ansi.o chars.o marinetti.o display.o

OPTIMIZE *= 79

CFLAGS	= -w -O$(OPTIMIZE)

$(PROG): $(OBJS)
	$(RM) ansi.root
	$(RM) chars.root
	$(CC) $(CFLAGS) $(OBJS) -o $@


fctelnet.o:	fctelnet.c marinetti.h
vt100.o:	vt100.c
ansi.o:		ansi.asm
chars.o:	chars.asm
marinetti.o:	marinetti.c

clean:
	$(RM) -f *.o *.root *.a *.r
clobber: clean
	$(RM) -f $(PROG)
