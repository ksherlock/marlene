#pragma noroot
#pragma lint -1

#include <types.h>
#include <string.h>
#include <ctype.h>
#include <Event.h>
#include "Marinetti.h"

#define ESC "\x1b"

extern void ScrollRegion(word,word);
extern void ScrollDown(void);
extern void PrintChar(int the_char, unsigned int andMask);
extern void ClearScreen(void);
extern void ClearScreenFrom(int Y);
extern void ClearLine(int Y);
extern void ClearLineFrom(int Y, int X);

void handle_ansi(word code, char *buffer, word cnt);

extern void GrafOn(void)	inline(0x0a04, 0xe10000);
extern void GrafOff(void)	inline(0x0b04, 0xe10000);
extern pascal void SetColorTable(Word, ColorTable) inline(0x0E04,dispatcher);
extern pascal void SysBeep2(Word) inline(0x3803,dispatcher);
extern pascal void SetAllSCBs(Word) inline(0x1404,dispatcher);
extern pascal void SetMasterSCB(Word) inline(0x1604,dispatcher);
extern pascal void InitColorTable(ColorTable) inline(0x0D04,dispatcher);



extern word Xpos;
extern word Ypos;
extern word __scroll[2];
extern word and_mask;
extern word xor_mask;

word esc;
char esc_buffer[256];

struct errlist
{
	Word Error;
	Word MsgLen;
	const char *Msg;
};

#define _ERR(n,msg) { n, sizeof(msg), msg }

struct errlist errors[] =
{
	_ERR(1,"tcperrDeafDestPort"),
	_ERR(2,"tcperrHostReset"),
	_ERR(3,"tcperrConExists"),
	_ERR(4,"tcperrConIllegal"),
	_ERR(5,"tcperrNoResources"),
	_ERR(6,"tcperrNoSocket"),
	_ERR(7,"tcperrBadPrec"),
	_ERR(8,"tcperrBadSec"),
	_ERR(9,"tcperrBadConnection"),
	_ERR(0x0a,"tcperrConClosing"),
	_ERR(0x0b,"tcperrClosing"),
	_ERR(0x0c,"tcperrConReset"),
	_ERR(0x0d,"tcperrUserTimeout"),
	_ERR(0x0e,"tcperrConRefused"),

	{0,0,NULL}
};
void display_str(const char *buffer, word cnt);

void display_err(Word err)
{
struct errlist *l;

	for (l = &errors[0]; l->Error; l++)
		if (l->Error == err) break;
	if (l->Error != err) return;

        display_str("\r\n" "\x1b" "[1m", 6); 		// bold on
	display_str(l->Msg, l->MsgLen); 	// show the string
	display_str("\x1b" "[0m", 4);	// bold off
}

void display_cstr(const char *cstr)
{
	if (cstr && *cstr)
		display_str(cstr, strlen(cstr));
}

void display_pstr(const char *pstr)
{
	if (pstr && *pstr)
		display_str(pstr + 1, *pstr);
}


void display_char(char c)
{
	switch(c)
	{
	case 0x07:
		SysBeep2(0);
		break;

	//backspace
	case 0x08:
		if (Xpos) Xpos--;
		break;

	//tab
	case 0x09: 
		// 1, 9, 17, 25, ...
		// not sure this is right
		Xpos = Xpos & 0xf8;
		Xpos += 8;
		//TODO - wrapping
		break;

	// line feed
	case 0x0A:
		if (++Ypos > 23)
		{
			ScrollRegion(__scroll[0], __scroll[1]);
			Ypos = 23;
		}
		break;


	case 0x0D: //carriage return
		Xpos = 0;
		break;

	default:
		if (isprint(c));
		{
			// check the wrapping flag ...
			if (Xpos > 80)
			{
				Xpos = 0;
				if (++Ypos > 23)
				{
					ScrollRegion(__scroll[0],
						__scroll[1]);

					Ypos = 23;
				}
			}
			PrintChar(c, and_mask);
			Xpos++;
		}
	}
}

/*
 * this reads text & has it drawn on the shr screen
 *
 */
// todo - and_mask + xor_mask
void display_str(const char *buffer, word cnt)
{
word i;


	asm
	{
		phk
		plb
	}

	for ( i = 0; i < cnt; i++)
	{
		if (esc)
		{
			esc_buffer[esc++] = buffer[i];
			if (isalpha(buffer[i])
				|| buffer[i] == '~'
				|| buffer[i] == '='
				|| buffer[i] == '>'
			)
			{
				handle_ansi(buffer[i],
				esc_buffer, esc);
				esc = 0;
			}
		}
		else switch (buffer[i])
		{
		// bell
		case 0x07:
			SysBeep2(0);
			break;

		//backspace
		case 0x08:
			if (Xpos) Xpos--;
			break;

		//tab
		case 0x09: 
					// 1, 9, 17, 25, ...
					// not sure this is right
					Xpos = Xpos & 0xf8;
					Xpos += 8;
					//TODO - wrapping
					break;

				// line feed
				case 0x0A:
					if (++Ypos > 23)
					{
						ScrollRegion(__scroll[0],
							__scroll[1]);

						Ypos = 23;
					}
					break;


				case 0x0D: //carriage return
					Xpos = 0;
					break;

				case 0x1b: //start of escape sequence
					
					esc = 1;
					esc_buffer[0] = 0x1b;
					break;

			default:
				if (isprint(buffer[i]))
				{
					// check the wrapping flag ...
					if (Xpos > 80)
					{
						Xpos = 0;
						if (++Ypos > 23)
						{
						ScrollRegion(__scroll[0],
							__scroll[1]);

						Ypos = 23;
						}
					}
					PrintChar(buffer[i], and_mask);
					Xpos++;
				}
		}
	}
}

//
// remap the iigs key to a vt100 code (if necessary) and send it out.

void send_event(EventRecord *event, Word ipid)
{
char *cp;
int len = 0;
word key;
word mods;

	key = event->message;
	mods = event->modifiers;


	if (mods & keyPad)
	{
		switch (key)
		{
		case 0x72: // insert
			cp = ESC "[2~";
			len = 4;
			break;

		case 0x75: // delete
			cp = ESC "[3~";
			len = 4;
			break;

		case 0x73: // home
			cp = ESC "[1~";
			len = 4;
			break;

		case 0x77: // end
			cp = ESC "[4~";
			len = 4;
			break;

		case 0x74: // page up
			cp = ESC "[5~";
			len = 4;
			break;

		case 0x79: // page down
			cp = ESC "[6~";
			len = 4;
			break;
		
		case 0x7a: // f1
			cp = mods & shiftKey ? ESC "[23~" : ESC "[11~";
			len = 5;
			break;

		case 0x78: // f2
			cp = mods & shiftKey ? ESC "[24~" : ESC "[12~";
			len = 5;
			break;

		case 0x63: // f3
			cp = mods & shiftKey ? ESC "[25~" : ESC "[13~";
			len = 5;
			break;

		case 0x76: // f4
			cp = mods & shiftKey ? ESC "[26~" : ESC "[14~";
			len = 5;
			break;


		case 0x60: // f5
			cp = mods & shiftKey ? ESC "[28~" : ESC "[15~";
			len = 5;
			break;

		case 0x61: // f6
			cp = mods & shiftKey ? ESC "[29~" : ESC "[17~";
			len = 5;
			break;


		case 0x62: // f7
			cp = mods & shiftKey ? ESC "[31~" : ESC "[18~";
			len = 5;
			break;

		case 0x64: // f8
			cp = mods & shiftKey ? ESC "[32~" : ESC "[19~";
			len = 5;
			break;

		case 0x65: // f9
			cp = mods & shiftKey ? ESC "[33~" : ESC "[20~";
			len = 5;                                    
			break;

		case 0x6d: // f10
			cp = mods & shiftKey ? ESC "[34~" : ESC "[21~";
			len = 5;
			break;

		case 0x67: // f11
			cp = ESC "[23~";
			len = 5;
			break;

		case 0x6f: // f12
			cp = ESC "[24~";
			len = 5;
			break;

		default: len = 0; cp = NULL;
		}
	}
	else if (mods & controlKey)
	{
			cp = (char *)&event->message;
			len = 1;
	}
	else
	{
		switch(key)
		{
		case 0x0d:
			cp = "\r\n";
			len = 2;
			break;
		case 0x08:	// <--- arrow
			cp = ESC "[D";
			len = 3;
			break;
		case 0x0A:	// down arrow
			cp = ESC "[B";
			len = 3;
			break;
		case 0x0B:	// up arrow
			cp = ESC "[A";
			len = 3;
			break;
		case 0x15:	// ---> arrow
			cp = ESC "[C";
			len = 3;
			break;

		// backspace to delete char
		case 0x7f:
			cp = "\x08";
			len = 1;
			break;

		default:
			cp = (char *)&event->message;
			len = 1;
		}
	}

	if (len) TCPIPWriteTCP(ipid, cp, len, false, false);

}
