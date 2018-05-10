PROG	= marlene
OBJS	= o/vt100.a o/telnet.a o/ansi.a o/chars.a o/marinetti.a o/display.a

CC = occ --gno

OPTIMIZE ?= 79

CFLAGS = -w-1 -O $(OPTIMIZE)
ASMFLAGS =

marlene: o/marlene.a $(OBJS)
	$(CC) $(OBJS) -o $@

darlene: o/darlene.a $(OBJS)
	$(CC) $(OBJS) -o $@



marlene.o: marlene.c
darlene.o: darlene.c
vt100.o: vt100.c CLAGS+=-r
marinetti.o: marinetti.c CLAGS+=-r
telnet.o: telnet.c CLAGS+=-r
ansi.o: ansi.asm
chars.o: chars.asm

o :
	mkdir o

o/%.a : %.c | o
	$(CC) -c $(CFLAGS) -o $@ $^

o/%.a : %.asm | o
	$(CC) -c $(ASMFLAGS) -o $@ $^
	$(RM) o/$*.root


clean:
	$(RM) -rf o
clobber: clean
	$(RM) -f $(PROG)
