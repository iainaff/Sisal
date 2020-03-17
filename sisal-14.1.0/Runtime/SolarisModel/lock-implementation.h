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
 * Solaris threads
\************************************************************************/
#define LOCKSDEFINED
 
typedef mutex_t                 LOCK_TYPE;
typedef struct {mutex_t lock; cond_t cond; int count;} BARRIER_TYPE;
 
#define MY_SLOCK(lp)            mutex_lock(lp)
#define MY_SUNLOCK(lp)          mutex_unlock(lp)
#define MY_SINIT_LOCK(lp)       mutex_init(lp, USYNC_PROCESS, NULL)
 
#define MY_LOCK(lp)             mutex_lock(lp)
#define MY_UNLOCK(lp)           mutex_unlock(lp)
#define MY_INIT_LOCK(lp)        mutex_init(lp, USYNC_PROCESS, NULL)
 
#define MY_LOCK_BACKUP(lp)      mutex_lock(lp)
#define MY_UNLOCK_BACKUP(lp)    mutex_unlock(lp)
 
#define INIT_BARRIER(bp,limit)  bp = (BARRIER_TYPE*)MyInitBarrier(limit)
#define WAIT_BARRIER(bp)        MyBarrier(bp)

#define FLUSHALL
#define FLUSH(start,end)
#define FLUSHLINE(addr)
#define CACHESYNC
#define FFL

#endif
