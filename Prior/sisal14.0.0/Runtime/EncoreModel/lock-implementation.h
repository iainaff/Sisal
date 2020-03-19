#ifndef LOCK_IMPLEMENTATION_H
#define LOCK_IMPLEMENTATION_H

/**************************************************************************/
/* FILE   **************   lock-implementation.h   ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

/************************************************************************\
 * Encore
\************************************************************************/
#define LOCKSDEFINED

#include <parallel.h>
typedef LOCK          LOCK_TYPE;
typedef BARRIER       BARRIER_TYPE;

#define MY_SLOCK(lp)          spin_lock((lp))
#define MY_SUNLOCK(lp)        spin_unlock((lp))
#define MY_SINIT_LOCK(lp)     spin_unlock((lp))

#define MY_LOCK(lp)           spin_lock((lp))
#define MY_UNLOCK(lp)         spin_unlock((lp))
#define MY_INIT_LOCK(lp)      spin_unlock((lp))

#define MY_LOCK_BACKUP(lp)    spin_lock((lp))
#define MY_UNLOCK_BACKUP(lp)  spin_unlock((lp))

#define INIT_BARRIER(bp,limit) \
{ \
  (bp) = barrier_init( (BARRIER_TYPE*) SharedMalloc( SIZEOF(BARRIER_TYPE) ), \
                      limit, SPIN_BLOCK ); \
  if ( (bp) == NULL ) \
    SisalError( "INIT_BARRIER", "barrier_init FAILED" ); \
}

#define WAIT_BARRIER(bp) barrier(bp)

#define FLUSHALL
#define FLUSH(start,end)
#define FLUSHLINE(addr)
#define CACHESYNC
#define FFL

#endif
