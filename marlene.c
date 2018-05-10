#include <tcpip.h>
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

extern void ClearScreen(void);

extern pascal void GrafOn(void) inline(0x0a04, dispatcher);
extern pascal void GrafOff(void) inline(0x0b04, dispatcher);

extern pascal void SetColorTable(Word, ColorTable) inline(0x0E04,dispatcher);
extern pascal void SysBeep2(Word) inline(0x3803,dispatcher);
extern pascal void SetAllSCBs(Word) inline(0x1404,dispatcher);
extern pascal void SetMasterSCB(Word) inline(0x1604,dispatcher);
extern pascal void InitColorTable(ColorTable) inline(0x0D04,dispatcher);


Word ResolveHost(const char *name, cvtRecPtr cvt);
int WaitForStatus(word ipid, word status_mask);


void display_pstr(const char *);
void display_cstr(const char *);
void display_err(Word);

extern void telnet_init(void);
extern void telnet_process(void);

extern void vt100_init(void);
extern void vt100_process(const unsigned char *buffer, unsigned buffer_size);
extern void vt100_event(EventRecord *event);




Word MyID;
Word __gno;


#define ESC "\x1b"



#pragma databank 1

/*
 * called to print connect/disconnect messages from
 * Marinetti.  msg is a pstring.
 */
void printCallBack(const char *msg) {
	if (msg && *msg) {
		display_cstr(ESC "\n\r[1m");  // bold on
		display_pstr(msg);  // show the string
		display_cstr(ESC "[0m\n\r"); // bold off
	}
}
#pragma databank 0


static void screen_init(void) {
#define WHITE	0x0fff
#define YELLOW	0x0ff0
#define BLACK	0x0000
#define BLUE	0x000f
#define GREEN	0x0080
#define RED		0x0f00

	static ColorTable ct =
	{
		WHITE, // background
		RED,   // underline / blink
		BLUE,  // bold
		GREEN, // foreground

		WHITE, 
		RED,
		BLUE,
		GREEN,

		WHITE, 
		RED,	
		BLUE, 
		GREEN, 

		WHITE, 
		RED,
		BLUE,
		GREEN,
	};

	unsigned i;

	// linearize memory, enable shadowing.
	asm
	{
		phb
		pea 0xe0e0
		plb
		plb
		sep #0x20
		lda #0xC1
		tsb 0xc029
		lda #0x08
		trb 0xc035
		rep #0x20
		plb
	}

	SetMasterSCB(0xc080);
	SetAllSCBs(0xc080);
	for (i = 0; i < 16; i++)
		SetColorTable(i, ct);

	ClearScreen();
	GrafOn();
}

static char out_buffer[1024];
static char out_buffer_size = 0;

static word ipid = -1;

static void flush(void) {
	if (out_buffer_size) {
		TCPIPWriteTCP(ipid, out_buffer, out_buffer_size, true, false);
		out_buffer_size = 0;
	}
}



void send(char *data, unsigned size) {
	
	if (out_buffer_size + size > sizeof(out_buffer)) {
		flush();
	}
	if (size > sizeof(out_buffer) / 2) {
		TCPIPWriteTCP(ipid, data, size, true, false);
		return;
	}
	memcpy(out_buffer + out_buffer_size, data, size);
	out_buffer_size += size;
}


unsigned buffer_size;
unsigned char buffer[2048]; // 1500 is normal MTU size?


int main(int argc, char **argv) {

	static EventRecord event;
	static rrBuff rr;
	static char str[16];
	static cvtRec cvt;
	static QuitRecGS qDCB = {2, 0, 0x4000};


	Word iLoaded;
	Word iConnected;
	Word iStarted;
	Handle dpHandle = NULL;
	Handle shrHandle = NULL;
	Handle shdHandle = NULL;
	Word err;
	int ok;
	unsigned i;


	iLoaded = iStarted = iConnected = false;
	__gno = false;
	ipid = -1;

	MyID = MMStartUp();

	kernStatus();
	if (!_toolErr) __gno = true;


	// todo:keypad flag of some sort?
	for (i = 1; i < argc; ++i) {
		char *cp = argv[i];
		if (cp[0] != '-') break;
		if (strcmp(cp, "--vt52") == 0) {

		} else if (strcmp(cp, "--vt100") == 0) {

		} else {
			ErrWriteCString("Unknown option: ");
			ErrWriteCString(cp);
			ErrWriteCString("\r\n");
			return 1;
		}
	}
	argv += i;
	argc -= i;

	if (argc != 1) {
		ErrWriteCString("Usage: marlene host[:port]\r\n");
		return 1;
	}

	TextStartUp();
	SetOutputDevice(1,3);
	SetOutGlobals(0x7f, 0);


	dpHandle = NewHandle(0x0100, MyID,
		attrBank | attrPage |attrNoCross | attrFixed | attrLocked,
		0x000000);

	shdHandle = NewHandle(0x8000, MyID,
		attrAddr | attrFixed | attrLocked,
		(void *)0x012000);

	shrHandle = NewHandle(0x8000, MyID,
		attrAddr | attrFixed | attrLocked,
		(void *)0xe12000);

	if (!dpHandle || !shdHandle || !shrHandle) {
		ErrWriteCString("Unable to allocate memory.\r\n");
		goto _exit;
	}

	EMStartUp((Word)*dpHandle, 0x14, 0, 0, 0, 0, MyID);


	screen_init();

	vt100_init();


	TCPIPStatus();
	if (_toolErr) {
		display_cstr("Loading Marinetti...\n\r");
		LoadOneTool(0x36, 0x200); //load Marinetti
		if (_toolErr) {
			display_cstr("Unable to load Marinetti.\r\n");
			goto _exit;
		}
		iLoaded = true;
	}

	// Marinetti now loaded
	if (!TCPIPStatus()) {
		display_cstr("Starting Marinetti...\n\r");
		TCPIPStartUp();
		iStarted = true;
	}

	if (!TCPIPGetConnectStatus()) {
		display_cstr("Connecting Marinetti...\n\r");
		TCPIPConnect(printCallBack);
		if (_toolErr) {
			display_cstr("Unable to establish network connection.\r\n");
			goto _exit;
		}
		iConnected = true;
	}
	// marinetti is now connected

	if (!ResolveHost(argv[0], &cvt)) {
		display_cstr("Unable to resolve address.\r\n");
		goto _exit;
	}

	if (!cvt.cvtPort) cvt.cvtPort = 23;

	ipid = TCPIPLogin(MyID, cvt.cvtIPAddress, cvt.cvtPort, 0, 0x0040);

	TCPIPConvertIPToCASCII(cvt.cvtIPAddress, str, 0);
	display_cstr("Connecting to ");
	display_cstr(str);
	display_cstr("...\n\r");

	TCPIPOpenTCP(ipid);

	// wait for the connection to occur.
	ok = WaitForStatus(ipid, 1 << TCPSESTABLISHED);
	if (ok > 0) display_err(ok);
	if (ok != 0) goto _exit2;


	display_cstr("Connected.\n\r");

	telnet_init();

	for(;;) {
		static rrBuff rr;


		TCPIPPoll();
		
		rr.rrBuffCount = 0;
		err = TCPIPReadTCP(ipid, 0, (Ref)buffer, sizeof(buffer), &rr);
		// tcperrConClosing and tcperrClosing aren't fatal.

		buffer_size = rr.rrBuffCount;
		if (buffer_size) err = 0;
		if (err) {
			if (err == tcperrConClosing || err == tcperrClosing)
				display_cstr("\r\nTCP Connection Closed.\r\n");
			else
				display_err(err);
			goto _exit1;
		}

		if (buffer_size) {
			telnet_process();
		}
		if (buffer_size) {
			vt100_process(buffer, buffer_size);
		}

		GetNextEvent(everyEvent, &event);
		if (event.what == keyDownEvt) {

			unsigned char key = event.message;

			if (event.modifiers & appleKey) {
				switch (key) {
				case 'Q':	// quit
				case 'q':
					goto _exit1;
					break;
				case 'V':	// paste...
				case 'v':
					break;
				case 'Z':	// suspend (gnome)
				case 'z':
					if (__gno) {
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
			
			} else {
				vt100_event(&event);
			}
		}
		flush();
	}

_exit1:
	flush();

	display_cstr("\n\rClosing TCP Connection...\n\r");

	TCPIPCloseTCP(ipid);
	WaitForStatus(ipid, (1 << TCPSCLOSED) | (1 << TCPSTIMEWAIT));

_exit2:


_exit:

	if (ipid != -1) TCPIPLogout(ipid);

	if (iConnected)
		TCPIPDisconnect(false, printCallBack);

	if (iStarted)
		TCPIPShutDown();

	if (iLoaded)
		UnloadOneTool(0x36);


	// flush q
	FlushEvents(everyEvent, 0);
	display_cstr("\n\rPress any key to exit.\n\r");
	while (!GetNextEvent(keyDownMask | autoKeyMask, &event)) ;


	EMShutDown();
	DisposeHandle(dpHandle);
	DisposeHandle(shdHandle);
	DisposeHandle(shrHandle);

	GrafOff();
	TextShutDown();
	QuitGS(&qDCB);
	return 0;
}

