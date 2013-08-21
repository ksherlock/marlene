#include "Marinetti.h"
#include <Locator.h>
#include <Memory.h>
#include <texttool.h>
#include <gsos.h>
#include <Event.h>
#include <fcntl.h>

#include <sgtty.h>
#include <errno.h>
#include <gno/gno.h>
#include <gno/kerntool.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/ioctl.compat.h>

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "telnet.h"

extern void ScrollRegion(word,word);
extern void ScrollDown(void);
extern void PrintChar(int the_char, unsigned int andMask);
extern void ClearScreen(void);
extern void ClearScreenFrom(int Y);
extern void ClearLine(int Y);
extern void ClearLineFrom(int Y, int X);

extern void GrafOn(void)	inline(0x0a04, 0xe10000);
extern void GrafOff(void)	inline(0x0b04, 0xe10000);
extern pascal void SetColorTable(Word, ColorTable) inline(0x0E04,dispatcher);
extern pascal void SysBeep2(Word) inline(0x3803,dispatcher);
extern pascal void SetAllSCBs(Word) inline(0x1404,dispatcher);
extern pascal void SetMasterSCB(Word) inline(0x1604,dispatcher);
extern pascal void InitColorTable(ColorTable) inline(0x0D04,dispatcher);


Word MyID;
Word __gno;

Word modeTelnet = 1;
Word flagEcho = 1;	// if *I* echo


void init_ansi(void);

Word ResolveHost(const char *name, cvtRecPtr cvt);
Word GetChar(word ipid, unsigned char *c);
Word UngetChar(char c);


void display_str(const char *txt, word cnt);
void display_pstr(const char *);
void display_cstr(const char *);
void display_err(Word);

void send_event(EventRecord *, Word);


#pragma databank 1

/*
 * called to print connect/disconnect messages from
 * Marinetti.  msg is a pstring.
 */
void printCallBack(const char *msg)
{
word i;
	i = *msg;
	if (i)
	{
		display_str("\x1b" "[1m", 4); 		// bold on
		display_str(msg + 1, i); 		// show the string
		display_str("\x1b" "[0m\n\r", 6);	// bold off
	}
}
#pragma databank 0

extern word esc;
extern word Xpos;
extern word Ypos;

void init_screen(void)
{
#define WHITE	0x0fff
#define YELLOW	0x0ff0
#define BLACK	0x0000
#define BLUE	0x000f
#define GREEN	0x00f0
#define RED		0x0f00

static ColorTable ct =
{
	BLACK, // background
	RED,   // underline / blink
	BLUE,  // bold
	GREEN, // foreground

	BLACK, 
	RED,
	BLUE,
	GREEN,

	BLACK, 
	RED,	
	BLUE, 
	GREEN, 

	BLACK, 
	RED,
	BLUE,
	GREEN,
};
int i;


	// linearize memory, disable shadowing.
	asm
	{
		lda #0x00C1
		sep #0x20
		ora >0xe0c029
		sta >0xe0c029
		rep #0x20

		lda #0x0008
		sep #0x20
		ora >0xe0c035
		sta >0xe0c035
		rep #0x20

	}

	SetMasterSCB(0xc080);
	SetAllSCBs(0xc080);
	//InitColorTable(ct);
	for (i = 0; i < 15; i++)
		SetColorTable(i, ct);

	ClearScreen();
	GrafOn();
}


/*
 * this is (more or less) a state machine.
 * old state (and character) are passed in
 * new state is returned.
 *
 * 0 = starting state.
 */
#define SB_IAC	0xfaff
Word do_iac(Word state, char c, Word ipid)
{
static char buffer[64];
static int cnt;

	if (state == 0) return c == IAC ? IAC : 0;
	// FF must be escaped as FF FF within SB.
	// SB_IAC means the first FF has been processed.
	if (state == SB)
	{
		if (c == IAC) return SB_IAC;
		buffer[cnt++] = c;
		return SB;
	}

	if (state == SB_IAC)
	{
		// it was an escped FF
		if (c == IAC)
		{
			buffer[cnt++] = IAC;
			return SB;
		}
		else state = IAC; // process below...
	}
	if (state == IAC)
		switch (c)
		{
		case DONT:
		case DO:
		case WONT:
		case WILL:
			return c;
		case SB:
			cnt = 0;
			return c;
		case SE:
			// buffer is the data between
			// IAC SB <...> IAC SE
			if (buffer[0] == TELOPT_TTYPE
				&& buffer[1] == TELQUAL_SEND)
			{
				static char msg[] =
				{
				IAC, SB, TELOPT_TTYPE, TELQUAL_IS,
				'V', 'T', '1', '0', '0',
				IAC, SE
				};
				TCPIPWriteTCP(ipid, msg,
					sizeof(msg), true, false);
			}
			else if (buffer[0] == TELOPT_NAWS
				&& buffer[1] == TELQUAL_SEND)
			{
				static char msg[] =
				{
				IAC, SB, TELOPT_NAWS,
				0, 80, 0, 24,
				IAC, SE
				};
				TCPIPWriteTCP(ipid, msg,
					sizeof(msg), true, false);
			}
			else if (buffer[0] == TELOPT_TSPEED
				&& buffer[1] == TELQUAL_SEND)
			{
				static char msg[] =
				{
				IAC, SB, TELOPT_TSPEED,
				'9', '6', '0', '0', ',', '9', '6', '0', '0',
				IAC, SE
				};
				TCPIPWriteTCP(ipid, msg,
					sizeof(msg), true, false);
			}
			cnt = 0;
			return 0;
		default:
			return 0;
		}
	if (state == DO)
	{
		buffer[0] = IAC;
		buffer[1] = WONT;
		buffer[2] = c;
		if (c == TELOPT_TTYPE
			|| c == TELOPT_SGA
			|| c == TELOPT_NAWS
			|| c == TELOPT_TSPEED)
		{
			buffer[1] = WILL;
		}
		TCPIPWriteTCP(ipid, buffer, 3, true, false);
		
	}
	if (state == DONT)
	{
		buffer[0] = IAC;
		buffer[1] = WONT;
		buffer[2] = c;

		TCPIPWriteTCP(ipid, buffer, 3, true, false);
	}
	if (state == WILL || state == WONT)
	{
		buffer[0] = IAC;
		buffer[1] = DONT;
		buffer[2] = c;
		if (c == TELOPT_ECHO)
		{
			buffer[1] = DO;
		}
		cnt = 3;

		TCPIPWriteTCP(ipid, buffer, cnt, true, false);
	}

        return 0;
}
		

int main(int argc, char **argv)
{
static EventRecord event;
static srBuffer sr;
static rrBuff rr;
static char str[16];
static cvtRec cvt;
static QuitRecGS qDCB = {2, 0, 0x4000};


	Word iLoaded;
	Word iConnected;
	Word iStarted;
	Word Quit;
	Word ipid;
	Handle dp;


	int i;
	Word key;
	Word err;
	unsigned char c;
	int fd;
	int iac_state;


	Xpos = Ypos = 0;
	esc = 0;
	//iac = 0;
	iLoaded = iStarted = iConnected = false;
	__gno = false;


	MyID = MMStartUp();

	kernStatus();
	if (!_toolErr) __gno = true;

	TextStartUp();
	SetOutputDevice(1,3);
	SetOutGlobals(0x7f, 0);


	dp = NewHandle(0x0100, MyID,
		attrBank | attrPage |attrNoCross | attrFixed | attrLocked,
		0x000000);
	HLock(dp);
	EMStartUp((Word)*dp, 0x14, 0, 0, 0, 0, MyID);


	if (argc != 2)
	{
		ErrWriteCString("Usage: marlene host[:port]\r\n");
		goto _exit;
	}

	init_screen();

	init_ansi();


	TCPIPStatus();
	if (_toolErr)
	{
		display_str("\n\rLoading Marinetti...\n\r", 23);
		LoadOneTool(0x36, 0x200); //load Marinetti
		if (_toolErr)
		{
			ErrWriteCString("Unable to load Marinetti\r\n");
			goto _exit;
		}
		iLoaded = true;
	}

	// Marinetti now loaded
	if (!TCPIPStatus())
	{
		display_str("\n\rStarting Marinetti...\n\r", 24);
	        TCPIPStartUp();
		iStarted = true;
	}

	if (!TCPIPGetConnectStatus())
	{
		display_str("\n\rConnecting Marinetti...\n\r", 26);
		TCPIPConnect(printCallBack);
		if (_toolErr)
		{
			ErrWriteCString("Unable to establish network connection\r\n");
			goto _exit;
		}
		iConnected = true;
	}
	// marinetti is now connected

	if (!ResolveHost(argv[1], &cvt))
	{
		ErrWriteCString("Unable to resolve address\r\n");
		goto _exit;
	}

	if (!cvt.cvtPort) cvt.cvtPort = 23;

	ipid = TCPIPLogin(MyID, cvt.cvtIPAddress, cvt.cvtPort, 0, 0x0040);

	display_str("\n\rConnecting to ", 16);
	display_str(str, TCPIPConvertIPToCAscii(cvt.cvtIPAddress, str, 0));

	TCPIPOpenTCP(ipid);

	// wait for the connection to occur.
	while (1)
	{
		TCPIPPoll();
		err = TCPIPStatusTCP(ipid, &sr);
		if (err)
		{
			display_err(err);
			goto _exit1;
		}

		if (sr.srState == tcpsESTABLISHED) break;
		//if (sr.srState == tcpsCLOSED) goto _exit1;
		GetNextEvent(keyDownMask | autoKeyMask ,&event);
		if (event.what != keyDownEvt) continue;
		if (!(event.modifiers & appleKey))  continue;
		if ((Word)event.message == '.') goto _exit1;
		if ((Word)event.message == 'q') goto _exit1;
		if ((Word)event.message == 'Q') goto _exit1;
	}

	display_str("\n\rConnected\n\r", 13);


	fd = open ("tcp.log", O_TRUNC | O_WRONLY | O_CREAT, 0777);

	Quit = false;


	iac_state = 0;
	while (!Quit)
	{

		TCPIPPoll();
		
		// check for incoming data...
		// 0xffff = no data
		// 0 = data
		// otherwise = marinetti error.
	        err = GetChar(ipid, &c);

		if (err && (err != 0xffff))
		{
			display_err(err);
			Quit++;
			continue;
		}

		if (err == 0)
		{
			if (fd > 0) write(fd, &c, 1);
			if (iac_state)
				iac_state = do_iac(iac_state, c, ipid);
			else if (modeTelnet && c == IAC)
				iac_state = do_iac(0, c, ipid);
			else display_str(&c, 1);
		}

		GetNextEvent(keyDownMask | autoKeyMask, &event);
		if (event.what != keyDownEvt) continue;
		c = key = event.message;

		if (event.modifiers & appleKey)
		{
			switch (key)
			{
			case 'Q':	// quit
			case 'q':
				Quit++;
				break;
			case 'V':	// paste
			case 'v':
				break;
			case 'Z':	// suspend (gnome)
			case 'z':
				if (__gno)
				{
				static struct sgttyb sb;
                                static int err;
					gtty(1,&sb);
					sb.sg_flags &= ~RAW;
					stty(1,&sb);
					GrafOff();
					Kkill(Kgetpid(), SIGSTOP, &err);
					sb.sg_flags |= RAW;
					stty(1,&sb);
					GrafOn();   
				}
				break;
			}
			continue;
		
		}
		else send_event(&event, ipid);

	}

_exit1:
	if (fd > 0) close(fd);
	TCPIPCloseTCP(ipid);
	TCPIPPoll(); // wait until closed...
	TCPIPLogout(ipid);

	// be nice and
	while (GetNextEvent(keyDownMask | autoKeyMask, &event));
	display_str("\n\rPress any key to exit.\n\r", 26);
	while (!GetNextEvent(keyDownMask | autoKeyMask, &event));


_exit:

	if (iConnected)
		TCPIPDisconnect(false, printCallBack);

	if (iStarted)
		TCPIPShutDown();

	if (iLoaded)
		UnloadOneTool(0x36);


	EMShutDown();
	GrafOff();
	TextShutDown();
	QuitGS(&qDCB);
	
}

