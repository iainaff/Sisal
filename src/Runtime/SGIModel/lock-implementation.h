#ifndef LOCK_IMPLEMENTATION_H
#define LOCK_IMPLEMENTATION_H

/**************************************************************************/
/* FILE   **************   lock-implementation.h   ************************/
/**************************************************************************/
/* Author: Dave Raymond                                                   */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

/************************************************************************\
 * SGI
\************************************************************************/
#define LOCKSDEFINED

typedef unsigned char LOCK_TYPE;
typedef char          BARRIER_TYPE;

BARRIER_TYPE *MyInitBarrier();

#define MY_SLOCK(lp)          MyLock((lp))
#define MY_SUNLOCK(lp)        MyUnlock((lp))
#define MY_SINIT_LOCK(lp)     MyInitLock((lp))

#define MY_LOCK(lp)           MyLock((lp))
#define MY_UNLOCK(lp)         MyUnlock((lp))
#define MY_INIT_LOCK(lp)      MyInitLock((lp))

#define MY_LOCK_BACKUP(lp)    MyLock((lp))
#define MY_UNLOCK_BACKUP(lp)  MyUnlock((lp))

#define INIT_BARRIER(bp,limit) bp = MyInitBarrier()
#define WAIT_BARRIER(bp)       MyBarrier(bp,NumWorkers)

#define FLUSHALL
#define FLUSH(start,end)
#define FLUSHLINE(addr)
#define CACHESYNC
#define FFL

#endif
