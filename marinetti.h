#ifndef __marinetti_h__
#define __marinetti_h__

#include <types.h>


enum {
  kLoaded = 1,
  kStarted = 2,
  kConnected = 4,

  kLoadError = -1,
  kVersionError = -2
};

Word ResolveHost(const char *name, cvtRecPtr cvt);
int WaitForStatus(word ipid, word status_mask);
int StartUpTCP(displayPtr fx);
void ShutDownTCP(int flags, Boolean force, displayPtr fx);

#endif

