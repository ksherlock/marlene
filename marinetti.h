#ifndef __TCPIP_H__
#define __TCPIP_H__

#include <types.h>

struct dnrBuffer
{
	Word DNRStatus;
	LongWord DNRIPAddress;
};
typedef struct dnrBuffer dnrBuffer, *dnrBufferPtr, **dnrBufferHndl;

#define DNRPending	0x0000
#define DNROK		0x0001
#define DNRFailed	0x0002
#define DNRNoDNSEntry	0x0003
#define DNRCancelled	0x0004

struct srBuffer
{
	Word srState;
	Word srNetworkError;
	LongWord srSndQueued;
	LongWord srRcvQueued;
	LongWord srDestIP;
	Word srDestPort;
	Word srConnectType;
	Word srAcceptCount;
};
typedef struct srBuffer srBuffer, *srBufferPtr, **srBufferHndl;

struct rrBuff
{
	LongWord rrBuffCount;
	Handle rrBuffHandle;
	Word rrMoreFlag;
	Word rrPushFlag;
	Word rrUrgentFlag;
};
typedef struct rrBuff rrBuff, *rrBuffPtr, **rrBuffHndl;

struct cvtRec
{
	LongWord cvtIPAddress;
	Word cvtPort;
};
typedef struct cvtRec cvtRec, *cvtRecPtr, **cvtRecHndl;


/*
 * TCP states
 */
#define tcpsCLOSED	0x0000
#define tcpsLISTEN	0x0001
#define tcpsSYNSENT	0x0002
#define tcpsSYNRCVD	0x0003
#define tcpsESTABLISHED	0x0004
#define tcpsFINWAIT1	0x0005
#define tcpsFINWAIT2	0x0006
#define tcpsCLOSEWAIT	0x0007
#define tcpsLASTACK	0x0008
#define tcpsCLOSING	0x0009
#define tcpsTIMEWAIT	0x000A


pascal void	TCPIPStartUp(void)	
	inline(0x0236 ,0xe10000);
pascal void	TCPIPShutDown(void)	
	inline(0x0336 ,0xe10000);

pascal Word	TCPIPStatus(void)
	inline(0x0636 ,0xe10000);

pascal Word TCPIPGetConnectStatus(void)
	inline(0x0936, 0xe10000);


pascal void	TCPIPConvertIPToHex(cvtRecPtr, const char *)	
	inline(0x0d36 ,0xe10000);
pascal void	TCPIPConvertIPCToHex(cvtRecPtr, const char *)	
	inline(0x3f36 ,0xe10000);

pascal Word	TCPIPConvertIPToAscii(LongWord, const char *, Word)	
	inline(0x0e36 ,0xe10000);
pascal Word	TCPIPConvertIPToCAscii(LongWord, const char *, Word)	
	inline(0x5836 ,0xe10000);

pascal Word	TCPIPMangleDomainName(Word, char *)
	inline(0x5936 ,0xe10000);

pascal Word	TCPIPValidateIPString(const char *)
	inline(0x4836 ,0xe10000);
pascal Word	TCPIPValidateIPCString(const char *)
	inline(0x1536 ,0xe10000);





pascal void	TCPIPConnect(void *)	
	inline(0x1236 ,0xe10000);
pascal void	TCPIPDisconnect(word, void *)	
	inline(0x1336 ,0xe10000);

pascal void	TCPIPCancelDNR(dnrBufferPtr)
	inline(0x2036 ,0xe10000);
pascal void	TCPIPDNRNameToIP(const char *, dnrBufferPtr)
	inline(0x2136 ,0xe10000);

pascal void	TCPIPPoll(void)	
	inline(0x2236 ,0xe10000);

pascal Word	TCPIPLogin(Word, LongWord, Word, Word, Word)	
	inline(0x2336 ,0xe10000);
pascal void	TCPIPLogout(Word)	
	inline(0x2436 ,0xe10000);

pascal Word	TCPIPOpenTCP(Word)	
	inline(0x2C36 ,0xe10000);
pascal Word	TCPIPWriteTCP(Word, void *, LongWord, Word, Word)	
	inline(0x2D36 ,0xe10000);
pascal Word	TCPIPReadTCP(Word, Word, Ref, LongWord, rrBuffPtr)
	inline(0x2E36 ,0xe10000);


pascal Word	TCPIPCloseTCP(Word)	
	inline(0x2F36 ,0xe10000);


pascal Word	TCPIPStatusTCP(Word, srBufferPtr)
	inline(0x3136, 0xe10000);






#endif

