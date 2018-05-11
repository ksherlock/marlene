#pragma noroot
#pragma lint -1

#include <Event.h>
#include <tcpip.h>
#include <locator.h>

#include "marinetti.h"

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
		while (dnr.DNRstatus == DNR_Pending) {
			TCPIPPoll();
			GetNextEvent(keyDownMask | autoKeyMask, &event);
			if ((event.what == keyDownEvt)
				&& (event.modifiers & appleKey)
				&& ((Word)event.message == '.')) {
				TCPIPCancelDNR(&dnr);
				break;
			}
		}
		if (dnr.DNRstatus == DNR_OK) {
			cvt->cvtIPAddress = dnr.DNRIPaddress;
			cvt->cvtPort = port;
			free(pstr);
			return true;
		}
	}

	free(pstr);

	return false;
}

int WaitForStatus(word ipid, word status_mask) {
	static srBuff sr;
	EventRecord event;
	Word err;
	unsigned bits;

	for(;;) {
		TCPIPPoll();
		err = TCPIPStatusTCP(ipid, &sr);
		if (err) return err;

		bits = 1 << sr.srState;
		if (status_mask & bits) return 0;

		GetNextEvent(keyDownMask | autoKeyMask, &event);
		if (event.what != keyDownEvt) continue;
		if (!(event.modifiers & appleKey))  continue;
		if ((Word)event.message == '.') return -1;
		//if ((Word)event.message == 'q') return -1;
		//if ((Word)event.message == 'Q') return -1;
	}
	return 0;
}

int StartUpTCP(displayPtr fx)
{
  word status;
  word flags = 0;
  
  // TCPIP is an init, not a tool, so it should always
  // be loaded.
  
  status = TCPIPStatus();
  if (_toolErr) {
    LoadOneTool(54, 0x0300);
    if (_toolErr == toolVersionErr) return kVersionError;
    if (_toolErr) return kLoadError;

    status = 0;
    flags |= kLoaded;
  }


  // require 3.0b3
  if (TCPIPLongVersion() < 0x03006003) {
    if (flags & kLoaded)
      UnloadOneTool(54);

    return kVersionError;     
  }

  if (!status) {
    TCPIPStartUp();
    if (_toolErr) return kLoadError;
    flags |= kStarted;
  }

  status = TCPIPGetConnectStatus();
  if (!status) {
    TCPIPConnect(fx);
    flags |= kConnected;
  }

  return flags;
}


void ShutDownTCP(int flags, Boolean force, displayPtr fx) {
  if (flags <= 0) return;

  if (flags & kConnected)
  {
    TCPIPDisconnect(force, fx);
    if (_toolErr) return;
  }
  if (flags & kStarted) {
    TCPIPShutDown();
    if (_toolErr) return;
  }
  if (flags & kLoaded) {
    UnloadOneTool(54);
  }
}

