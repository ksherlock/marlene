PROG	= marlene
OBJS	= main.o vt100.o telnet.o ansi.o chars.o marinetti.o display.o

OPTIMIZE *= 79

CFLAGS	= -w -O$(OPTIMIZE)

$(PROG): $(OBJS)
	$(RM) ansi.root
	$(RM) chars.root
	$(CC) $(CFLAGS) $(OBJS) -o $@


main.o:		main.c
vt100.o:	vt100.c
ansi.o:		ansi.asm
chars.o:	chars.asm
marinetti.o:	marinetti.c
telnet.o:	telnet.c
display.o:	display.c

clean:
	$(RM) -f *.o *.root *.a *.r
clobber: clean
	$(RM) -f $(PROG)
