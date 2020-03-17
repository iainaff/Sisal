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
 * IBM Power4
\************************************************************************/
#define LOCKSDEFINED

typedef int LOCK_TYPE;
typedef int BARRIER_TYPE;

#define MY_SLOCK(lp)           p6k_pllock((lp))
#define MY_SUNLOCK(lp)         p6k_plfree((lp))
#define MY_SINIT_LOCK(lp)      p6k_lockasgn((lp))
#define MY_INIT_LOCK(lp)       p6k_lockasgn((lp))

#define MY_LOCK(lp)            p6k_pllock((lp))
#define MY_UNLOCK(lp)          p6k_plfree((lp))

#define MY_LOCK_BACKUP(lp)     p6k_pllock((lp))
#define MY_UNLOCK_BACKUP(lp)   p6k_plfree((lp))

#define INIT_BARRIER(bp,limit) p6k_barasgn(bp,&(limit))
#define WAIT_BARRIER(bp)       p6k_barrier(bp)

#define FLUSHALL p6k_flush_all()
#define FLUSHLINE(addr) p6k_flush_line((addr))
#define CACHESYNC dcs()
#define FLUSH(start,end) p6k_flush_region((start),(end))

#define FFL(addr) p6k_flush_line((addr))

#endif
