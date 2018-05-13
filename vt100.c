/*
 * This handles vt100 commands.
 *
 */
#pragma noroot

#include <Event.h>
#include <ctype.h>

#define ESC "\x1b"
#define send_str(x) send((const unsigned char *)x, sizeof(x)-1)

extern void send(const unsigned char *, unsigned);

extern unsigned char buffer[];
extern unsigned buffer_size;



extern void ReverseScrollRegion(unsigned, unsigned);
extern void ScrollRegion(unsigned, unsigned);
extern void PrintChar(unsigned x, unsigned y, unsigned the_char, unsigned andMask, unsigned eorMask);

extern void ClearScreen(void);
extern void ClearScreen2(unsigned start, unsigned end);
extern void ClearLine(unsigned line);
extern void ClearLine2(unsigned line, unsigned start, unsigned end);

extern void SetCursor(unsigned x, unsigned y);

extern void HideCursor(void);
extern void ShowCursor(unsigned x, unsigned y);

extern pascal void SysBeep2(Word) inline(0x3803,dispatcher);


int __x;
int __y;


#define __brk() asm { brk 0xea }


static unsigned saved_cursor[2]; // saved cursor position
static unsigned window[2];
static unsigned and_mask;
static unsigned xor_mask;


static unsigned DECANM = 1;
static unsigned DECAWM = 1;
static unsigned DECCKM = 0;
static unsigned DECKPAM = 0;
static unsigned DECCOLM = 80;
static unsigned DECOM = 0;
static unsigned DECSCNM = 0;
static unsigned LNM = 0;

unsigned tabs[9] = { 0 };

static unsigned parms[10];
static unsigned parm_count;
static unsigned private;
static unsigned state;

void vt100_init(void)
{
	__x = 0;
	__y = 0;
	saved_cursor[0] = 0;
	saved_cursor[1] = 0;
	window[0] = 0;
	window[1] = 23;
	and_mask = 0xffff;
	xor_mask = 0x0000;

	DECANM = 1;
	DECAWM = 1;
	DECCKM = 0;
	DECKPAM = 0;
	DECCOLM = 80;
	DECOM = 0;
	DECSCNM = 0;
	LNM = 0;

	tabs[0] = 0x8080;
	tabs[1] = 0x8080;
	tabs[2] = 0x8080;
	tabs[3] = 0x8080;
	tabs[4] = 0x0080;

	parms[0] = 0;
	parm_count = 0;
	private = 0;
	state = 0;

	HideCursor();
}

#if 0
void dump(const char * buffer, word cnt, int impl)
{
int i;
char tmp[] = " xx x";
char c;

	if ( impl) WriteCString("Unimplemented: ");
	for (i = 0; i < cnt; i++)
	{
		c = buffer[i];
		tmp[1] = "0123456789abcdef"[c >> 4];
		tmp[2] = "0123456789abcdef"[c & 0x0f];
		tmp[4] = isprint(c) ? c : '.';
		WriteCString(tmp);
	}
	WriteCString("\r\n");
}
#endif

static void cursor_left(void){
	unsigned n = parms[0];
	if (n == 0) n = 1;

	__x -= n;
	if (__x < 0) __x = 0;
}

static void cursor_right(void){
	unsigned n = parms[0];
	if (n == 0) n = 1;
	// edge case
	if (__x == 80) return;
	__x += n;
	if (__x >= 80) n = 79;
}

static void cursor_up(void) {
	unsigned n = parms[0];
	if (n == 0) n = 1;

	/* can't escape scrolling region */
	if (__y >= window[0]) {
		__y -= n;
		if (__y < window[0]) __y = window[0];
	} else {
		__y -= n;
		if (__y < 0) __y = 0;
	}

}

static void cursor_down(void) {
	unsigned n = parms[0];
	if (n == 0) n = 1;

	/* can't escape scrolling region */
	if (__y <= window[1]) {
		__y += n;
		if (__y > window[1]) __y = window[1];
	} else {
		__y += n;
		if (__y >= 24) __y = 23;
	}
}

static void cursor_position(void) {
	unsigned y = parms[0];
	unsigned x = parms[1];

	if (y) --y;
	if (x) --x;

	if (x >= 80) x = 79;
	if (DECOM) {
		y = window[0] + y;
		if (y > window[1]) y = window[1];
	} else {
		if (y >= 24) y = 23;
	}
	__x = x;
	__y = y;
}

static void cursor_position_vt52(void) {
	unsigned y = parms[0];
	unsigned x = parms[1];

	if (y) --y;
	if (x) --x;

	// if x or y are out of range, do not move them.

	if (x < 80) __x = x;

	if (DECOM) {
		y = window[0] + y;
		if (y <= window[1]) __y = y;
	} else {
		if (y < 24) __y = y;
	}
}


static void scrolling_region(void) {
	unsigned top = parms[0];
	unsigned bottom = parms[1];

	if (top) --top;
	if (!bottom) bottom = 24;
	--bottom;


	if ((top < bottom) && (bottom < 24)) {
		window[0] = top;
		window[1] = bottom;
		__y = DECOM ? top : 0;
		__x = 0;
	}
}


static void set_attributes(void) {
	unsigned i;
	for (i = 0; i < parm_count; ++i) {
		switch(parms[i]) {
			case 0: // all attributes off
				and_mask = 0xffff;
				xor_mask = 0x0000;
				break;
			case 1: //bold
				and_mask = 0xaaaa;
				break;
			case 4: //underscore
			case 5: // blink
				and_mask = 0x5555;
				break;
			case 7://reverse video
				xor_mask = 0xffff;
				break;
		}
	}
}

static void set_mode(void) {

	unsigned i;
	for (i = 0; i < parm_count; ++i) {
		switch(parms[i]) {
			case 1: if (private) DECCKM = 1; break;
			case 2: if (private) DECANM = 1; break;
			case 3: if (private) DECCOLM = 132; break;
			case 6: if (private) { DECOM = 1; __x = 0; __y = window[0]; } break;
			case 7: if (private) DECAWM = 1; break;
			case 20: if (!private) LNM = 1; break;
		}
	}
}

static void reset_mode(void) {

	unsigned i;
	for (i = 0; i < parm_count; ++i) {
		switch(parms[i]) {
			case 1: if (private) DECCKM = 0; break;
			case 2: if (private) DECANM = 0; break;
			case 3: if (private) DECCOLM = 80; break;
			case 6: if (private) { DECOM = 0; __x = 0; __y = 0; } break;
			case 7: if (private) DECAWM = 0; break;
			case 20: if (!private) LNM = 0; break;
		}
	}
}

static void save_cursor(void) {
	saved_cursor[0] = __y;
	saved_cursor[1] = __x;
}

static void restore_cursor(void) {
	__y = saved_cursor[0];
	__x = saved_cursor[1];
	// vt220+ also saves/restores DECAWM and DECOM...
}

static void tab(void) {
	unsigned chunk;
	unsigned mask;

	if (__x >= 80) return;

	__x = __x + 1;
	chunk = __x >> 0x4;
	mask = 1 << (__x & 0x0f);

	while (chunk < 5) {
		unsigned bits = tabs[chunk];
		if (!bits) {
			__x += 16;
		} else {
			do {
				if (bits & mask) return;
				++__x;
				mask <<= 1;
			} while (mask);
		}
		mask = 1;
		++chunk;
	}
	// nothing... go to right margin.
	__x = 79;
}


static void set_tab(void) {
	unsigned chunk;
	unsigned mask;

	if (__x >= 80) return;

	chunk = __x >> 0x04;
	mask = 1 << (__x & 0x0f);

	tabs[chunk] |= mask;
}

static void clear_tabs(void) {
	unsigned i;	

	unsigned chunk;
	unsigned mask;

	for (i = 0; i < parm_count; ++i) {
		switch(parms[i]) {
			case 0:
				if (__x < 80) {
					chunk = __x >> 0x04;
					mask = 1 << (__x & 0x0f);
					tabs[chunk] &= ~mask;		
				}
				break;
			case 5:
				tabs[0] = 0;
				tabs[1] = 0;
				tabs[2] = 0;
				tabs[3] = 0;
				tabs[4] = 0;
				break;
		}
	}

}

static void linefeed(void) {
	if (__y == window[1]) {
		ScrollRegion(window[0], window[1]+1);
	} else ++__y;
}

static void reverse_linefeed(void) {
	if (__y == window[0]) {
		ReverseScrollRegion(window[0], window[1]+1);
	} else --__y;
}

static void erase_line(void) {
	unsigned i;
	for (i = 0; i < parm_count; ++i) {
		switch(parms[i]) {
			case 0:
				ClearLine2(__y, __x, 80);
				break;
			case 1: 
				ClearLine2(__y, 0, __x + 1);
				break;
			case 2:
				ClearLine(__y);
				break;
		}
	}
}

static void erase_screen(void) {
	unsigned i;

	for (i = 0; i < parm_count; ++i) {
		switch(parms[i]) {
			case 0:
				ClearLine2(__y, __x, 80);
				ClearScreen2(__y + 1, 24);
				break;
			case 1: 
				ClearLine2(__y, 0, __x + 1);
				ClearScreen2(0, __y);
				break;
			case 2:
				ClearScreen();
				break;
		}
	}
}

/* Device Status Report */
static void dsr(void) {
	unsigned i;
	for (i = 0; i < parm_count; ++i) {
		switch(parms[i]) {
			case 0: send_str(ESC "[0n"); break;
			case 5: { /* cursor position */
				static char buffer[16];
				unsigned i = 15;
				unsigned x = __x + 1;
				unsigned y = __y + 1;
				if (DECOM) y += window[0];
				buffer[i--] = ']';
				while(x) {
					buffer[i--] = '0' + (x % 10);
					x = x / 10;
				}
				buffer[i--] = ';';
				while(y) {
					buffer[i--] = '0' + (y % 10);
					y = y / 10;
				}
				buffer[i--] = '[';
				buffer[i] = 0x1b; 
				send(buffer + i, 16 - i);
				break;
			}
		}
	}
}

enum {
	st_text,
	st_escape,
	st_escape_v52,
	st_dca1,
	st_dca2,
	st_pound,
	st_rparen,
	st_lparen,
	st_lbracket,
	st_lbracket2,
	st_parm,
};

static void do_control(char c) {
	switch(c) {
		case 005: /* ENQ */ break;
		case 007: /* BEL */ SysBeep2(0); break;
		case 010: /* BS */ if (__x) --__x; break;
		case 011: /* HT */ tab(); break;
		case 012: /* LF */
		case 013: /* VT */
		case 014: /* FF */
			linefeed();
			if (LNM) __x = 0;
			break;
		case 015: /* CR */ __x = 0; break;
		case 016: /* SO */ break;
		case 017: /* SI */ break;
		case 021: /* XON */ break;
		case 023: /* XOFF */ break;

		case 030: /* CAN */
		case 032: /* SUB */
			state = st_text; break;
		case 033: /* ESC */
			switch (state) {
				case st_text: state = DECANM ? st_escape : st_escape_v52; break;
				case st_escape_v52:
				case st_escape:
					break;
				default:
					state = st_text;
			}
			break;
	}
} 

void vt100_process(const unsigned char *buffer, unsigned buffer_size) {

	unsigned i;
	HideCursor();
	for (i = 0; i < buffer_size; ++i) {
		unsigned char c = buffer[i] & 0x7f;
		if (c == 0 || c == 0x7f) continue;
		if (c < 0x20) {
			do_control(c);
			continue;
		}
		switch (state) {
			case st_text:
				if (__x == 80) {
					if (DECAWM) {
						__x = 0;
						if (__y == window[1]) {
							ScrollRegion(window[0], window[1]+1);
						} else ++__y;
					} else {
						__x = 79;
					}
				}
				PrintChar(__x, __y, c, and_mask, xor_mask);
				++__x;

				break;

			case st_escape:
				switch (c) {
					case '#': state = st_pound; break; 
					case '(': state = st_lparen; break; 
					case ')': state = st_rparen; break; 
					case '[': state = st_lbracket; break;
					case 'E': /* next line */ __x = 0; /* drop through */
					case 'D': /* index */ linefeed(); break;
					case 'H': set_tab(); break;
					case 'M': /* reverse index */ reverse_linefeed(); break;
					case 'Z': send_str(ESC "[?1;0c"); break;
					case '7': save_cursor(); break;
					case '8': restore_cursor(); break;
					case '=': DECKPAM = 1; break;
					case '>': DECKPAM = 0; break;
					case 'c': vt100_init(); ClearScreen(); break;
					case '1': case '2': /* vt105 graphic stuff */ break;
					default:
						break;
				}
				if (state == st_escape) state = st_text;
				break;

			case st_pound:
				/* #8 -> fill with Es */
				state = st_text; break;
			case st_lparen:
			case st_rparen:
				state = st_text; break;
				break;

			case st_lbracket:
				private = 0;
				parm_count = 0;
				parms[0] = 0;
				parms[1] = 0;
				if (c == '?') { private = 1; state = st_parm; continue; }
				if (isdigit(c)) {
					parms[0] = c - '0';
					state = st_parm;
					break;
				}
				if (c == ';') {
					++parm_count;
					parms[parm_count] = 0;
					state = st_parm;
					break;
				}
				/* fall through */	
			case st_lbracket2:
				lbracket2:
				++parm_count;
				switch (c) {
					case 'A': cursor_up(); break;
					case 'B': cursor_down(); break;
					case 'C': cursor_right(); break;
					case 'D': cursor_left(); break;
					case 'H': case 'f': cursor_position(); break;
					case 'K': erase_line(); break;
					case 'J': erase_screen(); break;
					case 'c': if (parms[0] == 0) send_str(ESC "[?1;0c"); break;
					case 'g': clear_tabs(); break;
					case 'h': set_mode(); break;
					case 'l': reset_mode(); break;
					case 'm': set_attributes(); break;
					case 'n': dsr(); break;
					case 'r': scrolling_region(); break;
				}
				state = st_text;
				break;						

			case st_parm:
				if (isdigit(c)) {
					parms[parm_count] *= 10;
					parms[parm_count] += c - '0';
					continue;
				}
				if (c == ';') {
					if (parm_count < 10) ++parm_count;
					parms[parm_count] = 0;
					continue;
				}
				state = st_lbracket2;
				goto lbracket2;

			case st_escape_v52:
				parm_count = 0;
				parms[0] = 0;
				if (c == 'Y') { state = st_dca1; break; }
				switch (c) {
					case 'A': cursor_up(); break;
					case 'B': cursor_down(); break;
					case 'C': cursor_right(); break;
					case 'D': cursor_left(); break;
					case 'F': case 'G': break; /* graphics mode */
					case 'H': cursor_position(); break;
					case 'I': reverse_linefeed(); break;
					case 'J': erase_screen(); break;
					case 'K': erase_line(); break;
					case 'Z': send_str(ESC "/Z"); break;
					case '=': DECKPAM = 1; break;
					case '>': DECKPAM = 0; break;
					case '<': DECANM = 1; break;

				}
				state = st_text;
				break;
			case st_dca1:
				parms[0] = c - 0x20;
				state = st_dca2;
				break;
			case st_dca2:
				parms[1] = c - 0x20;
				cursor_position_vt52();
				state = st_text;
				break;

		}
	}

	ShowCursor(__x, __y);
}


//
// remap the iigs key to a vt100 code (if necessary) and send it out.

/*
 * see Apple IIgs Toolbox Ref Volume 1, 3-22. (ADB codes)
 */
void vt100_event(EventRecord *event) {
	unsigned char *cp;
	int len = 0;
	unsigned char key;
	word mods;

	key = event->message;
	mods = event->modifiers;


	/* iigs -> vt100
	 * clear -> PF1
	 * =     -> PF2
	 * /     -> PF3
	 * *     -> PF4
	 * +     -> -
	 * -     -> ,
	 * .     -> .
	 *
	 *
	 * win32 layout:
	 * numlock -> PF1
	 * /       -> PF2
	 * *       -> PF3
	 * -       -> PF4
	 * shift+  -> -
	 * +       -> ,
	 *
	 * ------
	 * option - , 1 2 3 4 to use normal keys as keypad.
	 */

	if (mods & keyPad) {
		switch (key) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.':
		case '-':
		case ',':
			{
				static unsigned char buffer[3] = { 0x1b, 'x', 'x' };
				if (DECKPAM) {
					cp = buffer; len = 3;
					buffer[2] = key | 0x40;
					buffer[1] = DECANM ? 'O' : '?';
				} else {
					len = 1;
					cp = &key;
				}
			}
			break;

		case 0x0d: // enter
			if (DECKPAM) {
				cp = DECANM ? ESC "OM" : ESC "?M";
				len = 3;
			} else goto enter;
			break;


		case 0x7a: // f1
			if (DECANM) { cp = ESC "OP"; len = 3; }
			else { cp = ESC "P"; len = 2; }
			break;

		case 0x78: // f2
			if (DECANM) { cp = ESC "OQ"; len = 3; }
			else { cp = ESC "Q"; len = 2; }
			break;

		case 0x63: // f3
			if (DECANM) { cp = ESC "OR"; len = 3; }
			else { cp = ESC "R"; len = 2; }
			break;

		case 0x76: // f4
			if (DECANM) { cp = ESC "OS"; len = 3; }
			else { cp = ESC "S"; len = 2; }
			break;

		/* Real IIgs keyboard - clear = 0x18, =, /, *  for PF1-4? */
		case 0x18: // clear - PF1 on IIgs keyboard
			if (0) {
				if (DECANM) { cp = ESC "OP"; len = 3; }
				else { cp = ESC "P"; len = 2; }				
			}
			break;

		case '=': // PF2 on IIgs keyboard
			if (0) {
				if (DECANM) { cp = ESC "OQ"; len = 3; }
				else { cp = ESC "Q"; len = 2; }				
			}
			break;

		case '/': // PF3 on IIgs keyboard
			if (0) {
				if (DECANM) { cp = ESC "OR"; len = 3; }
				else { cp = ESC "R"; len = 2; }
			} else {
				len = 1;
				cp = &key;
			}
			break;

		case '*': // PF4 on IIgs keyboard
			if (0) {
				if (DECANM) { cp = ESC "OS"; len = 3; }
				else { cp = ESC "S"; len = 2; }				
			} else {
				len = 1;
				cp = &key;
			}
			break;

		case '+': 
			// send as normal key.
			len = 1;
			cp = &key;
			break;

		default: return;
		}
	} else if (mods & controlKey) {
		cp = &key;
		len = 1;
	} else {
		switch(key) {
		case 0x0d:
		enter:
			if (LNM) { cp = "\r\n"; len = 2; }
			else { cp = "\r"; len = 1; }
			break;
		case 0x08:	// <--- arrow
			// todo -- cursor keys
			if (DECANM) { 
				cp = DECCKM ?  ESC "OD" : ESC "[D";
				len = 3;
			} else { cp = ESC "D"; len = 2; }
			break;
		case 0x0A:	// down arrow
			if (DECANM) { 
				cp = DECCKM ?  ESC "OB" : ESC "[B";
				len = 3;
			} else { cp = ESC "B"; len = 2; }
			break;
		case 0x0B:	// up arrow
			if (DECANM) { 
				cp = DECCKM ?  ESC "OA" : ESC "[A";
				len = 3;
			} else { cp = ESC "A"; len = 2; }

			break;
		case 0x15:	// ---> arrow
			if (DECANM) { 
				cp = DECCKM ?  ESC "OC" : ESC "[C";
				len = 3;
			} else { cp = ESC "C"; len = 2; }
			break;

		// backspace to delete char
		case 0x7f: // delete
			cp = "\x08";
			len = 1;
			break;

		default:
			if (key < 0x7f) {
				cp = &key; len = 1;
			}
		}
	}

	if (len) send(cp, len);
}
