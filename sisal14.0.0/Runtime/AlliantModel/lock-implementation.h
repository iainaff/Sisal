#ifndef LOCK_IMPLEMENTATION_H
#define LOCK_IMPLEMENTATION_H

/**************************************************************************/
/* FILE   **************   lock-implementation.h   ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

/************************************************************************\
 * Alliant
\************************************************************************/
#define LOCKSDEFINED

typedef unsigned char LOCK_TYPE;
typedef int           BARRIER_TYPE;

#define MY_SLOCK(lp)          lock((lp))
#define MY_SUNLOCK(lp)        unlock((lp))
#define MY_SINIT_LOCK(lp)     initialize_lock((lp))

#define MY_LOCK(lp)           lock((lp))
#define MY_UNLOCK(lp)         unlock((lp))
#define MY_INIT_LOCK(lp)      initialize_lock((lp))

#define MY_LOCK_BACKUP(lp)    lock((lp))
#define MY_UNLOCK_BACKUP(lp)  unlock((lp))

#define INIT_BARRIER(b,limit)
#define WAIT_BARRIER(b)

#define FLUSHALL
#define FLUSH(start,end)
#define FLUSHLINE(addr)
#define CACHESYNC
#define FFL

/*
 * $Log:
 */

#endif
