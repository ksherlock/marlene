/*
 * The original concept. Not the original code :)
 *
 */

#include <texttool.h>
#include <gsos.h>
#include <Event.h>
#include <fcntl.h>
#include <Locator.h>
#include <Memory.h>


#include <sgtty.h>
#include <libutil.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/ioctl.compat.h>


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
	PostEvent(0x8000,0);
}
#pragma databank 0

int main(int argc, char **argv) {

	int fd;
	int pid;


	signal(SIGCHLD,sigchild);
	pid = forkpty2(&fd, NULL, NULL, NULL);
	if ( pid < 0) {
		ErrWriteCString("Unable to forkpty.\r\n");
		return 1;
	}

	for(;;) {
		int fio = 0;
		ioctl(fd, FIONREAD, &fio);
		if (fio > 256) fio = 256;
		if (fio > 0) {
			fio = read(fd, buffer, fio);
			if (fio > 0) vt100_process(buffer, fio);
		}
		GetNextEvent(...);


		if (fio <= 0) {
			union wait wt;
			ok = waitpid(pid, &wt, WNOHANG);
			if (ok <= 0)
		}
		asm { cop 0x7f }
	}

	if (fd >= 0) close(fd);
	return 0;
}