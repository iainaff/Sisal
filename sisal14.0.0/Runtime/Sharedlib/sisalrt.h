#ifndef _SISALRT_H
#define _SISALRT_H

#include "sisalInfo.h"

extern void ShutDownDsa();
extern void ParseTracerCommand();
extern void InitDsa();
extern void InitSignalSystem();

#include "locks.h"
#include "process.h"
#include "fibre.h"
#include "arrays.h"
#include "interface.h"

#endif
