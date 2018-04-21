#pragma noroot

#include <string.h>
#include "Marinetti.h"


struct errlist {
	Word Error;
	Word MsgLen;
	const char *Msg;
};

#define ESC "\x1b"
#define _ERR(n,msg) { n, sizeof(msg) - 1, msg }

extern void vt100_process(const unsigned char *buffer, unsigned buffer_size);

static struct errlist errors[] = {
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
};

void display_err(Word err) {

	if (err == 0 || err >= 0x0f) return;
	--err;
	vt100_process("\r\n" ESC "[1m", 6); // bold on
	vt100_process(errors[err].Msg, errors[err].MsgLen);
	vt100_process(ESC "[0m", 4);	// bold off
}

void display_cstr(const char *cstr) {
	if (cstr && *cstr)
		vt100_process(cstr, strlen(cstr));
}

void display_pstr(const char *pstr) {
	if (pstr && *pstr)
		vt100_process(pstr + 1, *pstr);
}




