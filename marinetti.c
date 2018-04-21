#pragma noroot
#pragma lint -1

#include <Event.h>
#include "Marinetti.h"

#include <stdlib.h>
#include <string.h>

static char * __c2pstrMalloc(const char *cp)
{
Word len;
char *ret;

	len = strlen(cp);
	ret = (char *)malloc(len + 2);

	if (ret)
	{
		ret[0] = len;
		memmove(&ret[1], cp, len+1); // copy over NULL too
	}

	return ret;
}

//
// returns true if resolved, false if not
//
// name is a c-string

// todo - add /etc/host support

Word ResolveHost(const char *name, cvtRecPtr cvt)
{
dnrBuffer dnr;
char *pstr;
Word port;
EventRecord event;

	cvt->cvtIPAddress = 0;
	cvt->cvtPort = 0;

	if (!name || !*name) return false;

	pstr = __c2pstrMalloc(name);
	if (!pstr) return false;

	port = TCPIPMangleDomainName(0xf800, pstr);

	// check if name is a dotted decimal or a host...

	if (TCPIPValidateIPString(pstr))
	{
		TCPIPConvertIPToHex(cvt, pstr);
		cvt->cvtPort = port;
		free (pstr);
		return true;
	}
	else
	{
		TCPIPDNRNameToIP(pstr, &dnr);
		if (_toolErr) {
			free(pstr);
			return false;
		}
		while (dnr.DNRStatus == DNRPending) {
			TCPIPPoll();
			GetNextEvent(keyDownMask | autoKeyMask, &event);
			if ((event.what == keyDownEvt)
				&& (event.modifiers & appleKey)
				&& ((Word)event.message == '.')) {
				TCPIPCancelDNR(&dnr);
				break;
			}
		}
		if (dnr.DNRStatus == DNROK) {
			cvt->cvtIPAddress = dnr.DNRIPAddress;
			cvt->cvtPort = port;
			free(pstr);
			return true;
		}
	}

	free(pstr);

	return false;
}

int WaitForStatus(word ipid, word status) {
	static srBuffer sr;
	EventRecord event;
	Word err;

	for(;;) {
		TCPIPPoll();
		err = TCPIPStatusTCP(ipid, &sr);
		if (err) return err;

		if (sr.srState == status) return 0;
		GetNextEvent(keyDownMask | autoKeyMask, &event);
		if (event.what != keyDownEvt) continue;
		if (!(event.modifiers & appleKey))  continue;
		if ((Word)event.message == '.') return -1;
		//if ((Word)event.message == 'q') return -1;
		//if ((Word)event.message == 'Q') return -1;
	}
	return 0;
}


