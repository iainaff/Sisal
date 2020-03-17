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
 * Sequent
\************************************************************************/
#define LOCKSDEFINED

#include <parallel/parallel.h>
typedef slock_t    LOCK_TYPE;
typedef sbarrier_t BARRIER_TYPE;

#define MY_SLOCK(lp)          s_lock((lp))
#define MY_SUNLOCK(lp)        s_unlock((lp))
#define MY_SINIT_LOCK(lp)     s_init_lock((lp))

#define MY_LOCK(lp)           s_lock((lp))
#define MY_UNLOCK(lp)         s_unlock((lp))
#define MY_INIT_LOCK(lp)      s_init_lock((lp))

#define MY_LOCK_BACKUP(lp)    s_lock((lp))
#define MY_UNLOCK_BACKUP(lp)  s_unlock((lp))

#define INIT_BARRIER(b,np)    s_init_barrier(b,np)
#define WAIT_BARRIER(b)       s_wait_barrier(b)

#define FLUSHALL
#define FLUSH(start,end)
#define FLUSHLINE(addr)
#define CACHESYNC
#define FFL

#endif
