#ifndef SISALRT_H
#define SISALRT_H

/**************************************************************************/
/* FILE   **************         sisalrt.h         ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

#include "sisalInfo.h"

extern void ShutDownDsa PROTO((void));
extern void ParseTracerCommand PROTO((char*));
extern void InitDsa PROTO((int,int));
extern void InitSignalSystem PROTO((void));

extern int sisal_file_io;

#include "locks.h"
#include "process.h"
#include "fibre.h"
#include "arrays.h"
#include "interface.h"

#endif
