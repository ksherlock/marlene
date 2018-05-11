#include <Event.h>
#include <gsos.h>
#include <Locator.h>
#include <Memory.h>
#include <tcpip.h>
#include <texttool.h>


#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <gno/gno.h>
#include <gno/kerntool.h>
#include <sgtty.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/ioctl.compat.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "telnet.h"
#include "marinetti.h"


void display_pstr(const char *);
void display_cstr(const char *);
void display_err(Word);

extern void telnet_init(void);
extern void telnet_process(void);

extern void vt100_init(void);
extern void vt100_process(const unsigned char *buffer, unsigned buffer_size);
extern void vt100_event(EventRecord *event);

extern void screen_init(void);
extern void screen_on(void);
extern void screen_off(void);


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


	Handle dpHandle = NULL;
	Handle shrHandle = NULL;
	Handle shdHandle = NULL;
	Word err;
	int ok;
	unsigned i;
	int mf = 0;
	Word MyID;
	Word __gno;


	__gno = false;
	ipid = -1;

	MyID = MMStartUp();
	TextStartUp();
	SetOutputDevice(1,3);
	SetOutGlobals(0x7f, 0);

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

		if (dpHandle) DisposeHandle(dpHandle);
		if (shdHandle) DisposeHandle(shdHandle);
		if (shrHandle) DisposeHandle(shrHandle);

		return 1;
	}

	EMStartUp((Word)*dpHandle, 0x14, 0, 0, 0, 0, MyID);


	screen_init();

	vt100_init();

	mf = StartUpTCP(printCallBack);
	if (mf < 0) {
		display_cstr("Marinetti 3.0b3 or greater is required.\r\n");
		goto _exit;
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
	if (ok != 0) goto _exit;


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
						gtty(1,&sb);
						sb.sg_flags &= ~RAW;
						stty(1,&sb);
						screen_off();
						Kkill(Kgetpid(), SIGSTOP, &errno);
						sb.sg_flags |= RAW;
						stty(1,&sb);
						screen_on();
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


_exit:

	if (ipid != -1) TCPIPLogout(ipid);

	if (mf > 0) ShutDownTCP(mf, false, printCallBack);


	// flush q
	FlushEvents(everyEvent, 0);
	display_cstr("\n\rPress any key to exit.\n\r");
	while (!GetNextEvent(keyDownMask | autoKeyMask, &event)) ;


	EMShutDown();
	DisposeHandle(dpHandle);
	DisposeHandle(shdHandle);
	DisposeHandle(shrHandle);

	screen_off();
	TextShutDown();
	//QuitGS(&qDCB);
	return 0;
}

