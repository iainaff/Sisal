/************************************************************************\
 * locks.h - SISAL runtime system machine-specific lock definitions
\************************************************************************/

extern void MyInitLock();
extern void MyLock();
extern void MyUnlock();
extern void MyBarrier();

#include "lock-implementation.h"

