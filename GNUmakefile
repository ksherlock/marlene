PROG	= marlene
OBJS	= o/main.a o/vt100.a o/telnet.a o/ansi.a o/chars.a o/marinetti.a o/display.a

CC = occ --gno

OPTIMIZE ?= 79

CFLAGS = -w-1 -O $(OPTIMIZE)
ASMFLAGS =

$(PROG): $(OBJS)
	$(RM) o/ansi.root
	$(RM) o/chars.root
	$(CC) $(OBJS) -o $@


main.o: main.c
vt100.o: vt100.c CLAGS+=-r
ansi.o: ansi.asm
chars.o: chars.asm
marinetti.o: marinetti.c CLAGS+=-r
telnet.o: telnet.c CLAGS+=-r

o :
	mkdir o

o/%.a : %.c | o
	$(CC) -c $(CFLAGS) -o $@ $^

o/%.a : %.asm | o
	$(CC) -c $(ASMFLAGS) -o $@ $^


clean:
	$(RM) -rf o
clobber: clean
	$(RM) -f $(PROG)
