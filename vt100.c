#pragma noroot

#include <Types.h>
#include <texttool.h>
#include <ctype.h>
// handle vt100 escape codes

extern void ReverseScrollRegion(word, word);
extern void ScrollRegion(word, word);
extern void ScrollDown(void);
extern void PrintChar(int the_char, unsigned int andMask);
extern void ClearScreen(void);
extern void ClearScreenFrom(int Y);
extern void ClearLine(int Y);
extern void ClearLineFrom(int Y, int X);


word Xpos;
word Ypos;
static word __pos[2]; // saved cursor position
word __scroll[2];
word and_mask;
word xor_mask;


void init_ansi(void)
{
	Xpos = 0;
	Ypos = 0;
	__pos[0] = 0;
	__pos[1] = 0;
	__scroll[0] = 0;
	__scroll[1] = 23;
	and_mask = 0xffff;
	xor_mask = 0x0000;
}

//not yet added:
//esc[6n - return sursor position report
//esc[m - set graphics mode
//

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


void handle_ansi(word code, char *buffer, word cnt)
{
word i;
word r0;
word r1;
//word r2;
//word r3;
word pop = 0;


	if (buffer[1] == '[')
	{
		i = 2;

		switch (code)
		{
		case 'A': //cursor up
			// esc[#A
			r0 = 0;
			while (isdigit(buffer[i]))
				r0 = r0 * 10 + (buffer[i++] - '0');
			if (buffer[i] != 'A') pop = 1;
			else
			{
				if (!r0) r0 = 1;
				Ypos -= r0;
				if (Ypos < __scroll[0]) Ypos = __scroll[0];
				//if (Ypos > r0) Ypos -= r0;
				//else Ypos = 0;
			}
			break;

		case 'B': //cursor down
			// esc[#B
			r0 = 0;
			while (isdigit(buffer[i]))
			r0 = r0 * 10 + (buffer[i++] - '0');
			if (buffer[i] != 'B') pop = 1;
			else
			{
				if (!r0) r0 = 1;
				Ypos += r0;
				if (Ypos > __scroll[1]) Ypos = __scroll[1];
				//if (Ypos + r0 > 24) Ypos = 24;
				//else Ypos += r0;
			}
			break;

		case 'C': // cursor forward
			// esc[#C
			r0 = 0;
			while (isdigit(buffer[i]))
				r0 = r0 * 10 + (buffer[i++] - '0');
			if (buffer[i] != 'C') pop = 1;
			else
			{
				if (!r0) r0 = 1;
				Xpos += r0;
				if (Xpos > 79) Xpos = 79;
			}
			break;

		case 'D': // cursor backward
			// esc[#D
			r0 = 0;
			while (isdigit(buffer[i]))
				r0 = r0 * 10 + (buffer[i++] - '0');
			if (buffer[i] != 'D') pop = 1;
			else
			{
				if (!r0) r0 = 1;
				Xpos -= r0;
				if (Xpos < 0) Xpos = 0;
			}
			break;
                                   
		case 'H': //cursor position
		case 'f':
			//esc[#;#H or esc[;H or esc[H <?>
			//esc[#;#f or esc[;f or esc[f <?>
			if (cnt > 3)
			{
				r0 = r1 = 0;
				while (isdigit(buffer[i]))
					r0 = r0 * 10 + (buffer[i++] - '0');
				if (buffer[i++] != ';') pop = 1;
				else while (isdigit(buffer[i]))
					r1 = r1 * 10 + (buffer[i++] - '0');
				if (buffer[i] != 'H' && buffer[i] != 'f') pop = 1;
				if (r0 == 0) r0++;
				if (r1 == 0) r1++;
			}
			else r0 = r1 = 1;

			if (!pop)
			{
				Xpos = r1 - 1;
				Ypos = r0 - 1;
			}
			break;

		case 'J':
			if (cnt == 3)  //clear c
			{
				ClearLineFrom(Ypos, Xpos);
				if (Ypos < 23)
					ClearScreenFrom(Ypos+1);

			}
			else if (cnt == 4 && buffer[2] == '2')
			{
				ClearScreen();
				//Ypos = 0; // cursor position unchanged
				//Xpos = 0;
			}
			break;

		case 'K': //erase from cursor to end-of-line (including cursor)
			if (cnt == 3) // no parms
				ClearLineFrom(Ypos, Xpos);
			else dump(buffer, cnt,1);
			break;

		case 'L': // erase current line and
			// scroll preceding lines down 1
			// esc[#L
			ReverseScrollRegion(Ypos, __scroll[1]);
			break;

		case 'M': // erase current line and
			//scroll following lines up 1
			// esc[#M
			ScrollRegion(Ypos, __scroll[1]);
			break;			

		case 's': // save cursor position
			__pos[0] = Xpos;
			__pos[1] = Ypos;
			break;

		case 'u': // restore cursor position
			Xpos = __pos[0];
			Ypos = __pos[1];
			break;

		// not yet added!
		case 'n':
			break;
		case 'm':
			while (1)
			{
				r0 = 0;
				while (isdigit(buffer[i]))
					r0 = r0 * 10 + (buffer[i++] - '0');
		
				switch(r0)
				{
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
				case 8://cancel
					and_mask = 0x0000;
					break;
				}
				if (buffer[i] == ';') i++;
				else if (buffer[i] == 'm') break;
			}
			break;

		case 'r': // #;#r == set scrolling region
			// esc[#;#r
			r0 = r1 = 0;

			while (isdigit(buffer[i]))
				r0 = r0 * 10 + (buffer[i++] - '0');
			if (buffer[i++] != ';') pop = 1;
			else while (isdigit(buffer[i]))
				r1 = r1 * 10 + (buffer[i++] - '0');
			if (buffer[i] != 'r') pop = 1;
			if (r0 && r1 && !pop)
			{
				__scroll[0] = r0 - 1;
				__scroll[1] = r1 - 1;
			}
			break;

		default:
			dump(buffer, cnt,1);
		}
	}
	else
	{
		switch(buffer[1])
		{
		case 'M': // scroll screen down 1 line
			ReverseScrollRegion(__scroll[0], __scroll[1]);
			break;
		case '=':	// alt key pad
		case '>':	// normal key pad
			break;
		default:
			dump(buffer, cnt,1);

		}
	}
}

