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
 * POSIX threads
\************************************************************************/
#define LOCKSDEFINED
#include <pthread.h>

typedef pthread_mutex_t         LOCK_TYPE;
typedef struct {pthread_mutex_t lock; int count;} BARRIER_TYPE;
 
#define MY_SLOCK(lp)            pthread_mutex_lock(lp)
#define MY_SUNLOCK(lp)          pthread_mutex_unlock(lp)
#define MY_SINIT_LOCK(lp)       pthread_mutex_init(lp, NULL)
 
#define MY_LOCK(lp)             pthread_mutex_lock(lp)
#define MY_UNLOCK(lp)           pthread_mutex_unlock(lp)
#define MY_INIT_LOCK(lp)        pthread_mutex_init(lp, NULL)
 
#define MY_LOCK_BACKUP(lp)      pthread_mutex_lock(lp)
#define MY_UNLOCK_BACKUP(lp)    pthread_mutex_unlock(lp)
 
#define INIT_BARRIER(bp,limit)  bp = (BARRIER_TYPE*)MyInitBarrier(limit)
#define WAIT_BARRIER(bp)        MyBarrier(bp)

#define FLUSHALL
#define FLUSH(start,end)
#define FLUSHLINE(addr)
#define CACHESYNC
#define FFL

#endif
