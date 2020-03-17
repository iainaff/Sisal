#ifndef PROCESS_IMPLEMENTATION_H
#define PROCESS_IMPLEMENTATION_H
/**************************************************************************/
/* FILE   **************  process-implementation.h ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

/* CrayModel */

extern int ProcessorId PROTO((void));
#define GetProcId ProcessorId()
#define GETPROCID(v) { int x; TSKVALUE(&x); v = x; }

#endif
