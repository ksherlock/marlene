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
		if (_toolErr)
		{
			free(pstr);
			return false;
		}
		while (dnr.DNRStatus == DNRPending)
		{
			TCPIPPoll();
			GetNextEvent(keyDownMask | autoKeyMask, &event);
			if ((event.what == keyDownEvt)
				&& (event.modifiers & appleKey)
				&& ((Word)event.message == '.'))
			{
				TCPIPCancelDNR(&dnr);
				break;
			}
		}
		if (dnr.DNRStatus == DNROK)
		{
			cvt->cvtIPAddress = dnr.DNRIPAddress;
			cvt->cvtPort = port;
			free(pstr);
			return true;
		}
	}

	free(pstr);

	return false;
}


/*
 * pcnt is the next place to store data
 * bptr is the current ptr.
 */
static unsigned char buffer[1024];
static unsigned char pushback[1024];
static int pcnt = 0;
static int bcnt = 0;
static int bptr = 0;
static rrBuff rr;

Word GetChar(word ipid, unsigned char *c)
{
word err;

	*c = 0;

	if (pcnt)
	{
		*c = pushback[--pcnt];
	        return 0;
	}
	if (!bcnt)
	{
		err = TCPIPReadTCP(ipid, 0, (Ref)buffer, 1024, &rr);
		bcnt = rr.rrBuffCount;
		bptr = 0;
		if (err) return err;
	}	
	if (bcnt)
	{
		bcnt--;
		*c = buffer[bptr++];
		return 0;	
	}
	return 0xffff;
}

void UngetChar(char c)
{
	pushback[pcnt++] = c;
}

