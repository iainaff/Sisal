#ifndef _WORLD_LOADED
#define _WORLD_LOADED

#include "config.h"

extern void ShutDownDsa();
extern void ParseTracerCommand();
extern void InitDsa();
extern void InitSignalSystem();

#include "conf.h"
#include "locks.h"
#include "process.h"
#include "fibre.h"
#include "arrays.h"
#include "interface.h"

#endif
