COMMON_OBJS	= o/vt100.a o/ansi.a o/chars.a o/display.a o/screen.a

MARLENE_OBJS = o/marlene.a o/telnet.a o/marinetti.a 
DARLENE_OBJS = o/darlene.a

CC = occ --gno

OPTIMIZE ?= 79

CFLAGS = -w-1 -O $(OPTIMIZE)
ASMFLAGS =

.PHONY: all clean clobber

all: marlene darlene

marlene: $(MARLENE_OBJS) $(COMMON_OBJS)
	$(CC) -o $@ $^ 

darlene: $(DARLENE_OBJS) $(COMMON_OBJS)
	$(CC) -lutil -o $@ $^



marlene.o: marlene.c
darlene.o: darlene.c
vt100.o: vt100.c CLAGS+=-r
marinetti.o: marinetti.c CLAGS+=-r
telnet.o: telnet.c CLAGS+=-r
screen.o: screen.c CLAGS+=-r
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
	$(RM) -f marlene darlene
