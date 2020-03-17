#ifndef LOCKS_H
#define LOCKS_H

/**************************************************************************/
/* FILE   **************          locks.h          ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/
/* locks.h - SISAL runtime system machine-specific lock definitions       */
/**************************************************************************/

#include "lock-implementation.h"

extern void MyInitLock PROTO((void));
extern void MyLock PROTO((void));
extern void MyUnlock PROTO((void));
extern void MyBarrier PROTO((BARRIER_TYPE *));

#endif
