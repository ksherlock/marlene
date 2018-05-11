/*
 * The original concept. Not the original code :)
 *
 */

#include <Event.h>
#include <gsos.h>
#include <Locator.h>
#include <Memory.h>
#include <texttool.h>

#include <errno.h>
#include <fcntl.h>
#include <gno/gno.h>
#include <gno/kerntool.h>
#include <libutil.h>
#include <sgtty.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.compat.h>
#include <sys/ioctl.h>
#include <unistd.h>

extern void screen_init(void);
extern void screen_on(void);
extern void screen_off(void);


static char out_buffer[1024];
static char out_buffer_size = 0;

static int master = -1;

static void flush(void) {
	if (out_buffer_size) {
		write(master, out_buffer, out_buffer_size);
		out_buffer_size = 0;
	}
}



void send(char *data, unsigned size) {
	
	if (out_buffer_size + size > sizeof(out_buffer)) {
		flush();
	}
	if (size > sizeof(out_buffer) / 2) {
		write(master, data, size);
		return;
	}
	memcpy(out_buffer + out_buffer_size, data, size);
	out_buffer_size += size;
}


int 
openpty2(int *amaster, int *aslave, char *name, struct sgttyb *sg, struct winsize *winp) {

	char ptyname[] = ".ptyq0";
	char ttyname[] = ".ttyq0";

	int master;
	int slave;
	unsigned i;
	for (i = 0; i < 16; ++i) {
		ptyname[5] = ttyname[5] = "0123456789abcdef"[i];
		master = open(ptyname, O_RDWR);
		if (master < 0) continue;

		slave = open(ttyname, O_RDRW);
		if (slave < 0) {
			close(master);
			continue;
		}


		*amaster = master;
		*aslave = slave;

		if (name) strcpy(name, ttyname);
		if (sg) stty(slave, sg);
		if (winp) ioctl(slave, TIOCSWINSZ, (char *)winp);

		return 0;
	}
	errno = ENOENT;
	return -1;
}



#pragma databank 1
static int _child(int master, int slave) {

	close(master);

	logintty(slave);

	execve(":bin:gsh", "gsh -f");
	write(slave, "Unable to exec.\r\n", 27);
	close(slave);
	return 1;
}
#pragma databank 0

int
forkpty2(int *amaster, char *name, struct sgttyb *sg, struct winsize *winp) {

	int ok;
	int master, slave, pid;

	ok = openpty2(&master, &slave, name, sg, winp);
	if (ok < 0) return -1;

	pid = fork2(_child, 256, 0, NULL, 4, master, slave);
	if (pid < 0) {
		close(master);
		close(slave);
		return -1;
	}
	close(slave);
	return pid;
}

#pragma databank 1
static void sigchild(int sig, int x) {
	PostEvent(app4Mask,0);
}
#pragma databank 0

int main(int argc, char **argv) {

	int pid;
	unsigned i;

	Handle dpHandle = NULL;
	Handle shrHandle = NULL;
	Handle shdHandle = NULL;

	master = -1;
	kernStatus();
	if (_toolErr) {
		ErrWriteCString("GNO/ME required.\r\n");
		return 1;
	}

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

	argc -= i;
	argv += i;


	signal(SIGCHLD,sigchild);
	pid = forkpty2(&master, NULL, NULL, NULL);
	if ( pid < 0) {
		ErrWriteCString("Unable to forkpty.\r\n");
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


	screen_init();
	vt100_init();

	for(;;) {
		static char buffer[1024];
		int fio = 0;
		ioctl(master, FIONREAD, &fio);
		if (fio > sizeof(buffer)) fio = sizeof(buffer);
		if (fio > 0) {
			fio = read(master, buffer, fio);
			if (fio > 0) vt100_process(buffer, fio);
		}

		if (GetNextEvent(everyEvent, &event)) {
			fio = 1;
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
					case 'z': {
							/* EMStartUp puts the tty in RAW mode. */
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

			if (event.what == app4Mask) {
				/* child signal received! */
				union wait wt;
				ok = waitpid(pid, &wt, WNOHANG);
				if (ok <= 0) continue;
				display_str("\r\nChild exited.\r\n");
				break;
			}
		}
		flush();
		if (fio <= 0) asm { cop 0x7f }
	}
_exit1:
	flush();
	if (master >= 0) close(master);

_exit:
	// flush q
	FlushEvents(everyEvent, 0);
	display_cstr("\n\rPress any key to exit.\n\r");
	while (!GetNextEvent(keyDownMask | autoKeyMask, &event)) ;


	EMShutDown();
	DisposeHandle(dpHandle);
	DisposeHandle(shdHandle);
	DisposeHandle(shrHandle);

	screen_off();
	return 0;
}