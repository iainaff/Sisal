#ifndef PARALLELISM_H
#define PARALLELISM_H

/**************************************************************************/
/* FILE   **************       parallelism.h       ************************/
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

int p_procnum = 0;
static LOCK_TYPE  TheLock;

void ReleaseSharedMemory()
{
  free( SharedBase );
}

static void* Transfer( ProcId )
void* ProcId;
{
  GetProcId = (long)ProcId;

#if defined(DIST_DSA)
  if (ProcId != 0) {
    InitDsa(DsaSize/(2*(NumWorkers - 1)), XftThreshold);
  }
#endif

  EnterWorker( ProcId );

  if ( ProcId != 0 ) {
    LeaveWorker();
    thr_exit( 0 );
  }
  return NULL;
}

void AcquireSharedMemory( NumBytes ) 
int NumBytes;
{
  SharedSize = NumBytes + 100000;

  SharedBase = SharedMemory = (char *) malloc( SharedSize-40 );

  if ( SharedMemory == (char *) NULL )
    SisalError( "AcquireSharedMemory", "malloc FAILED" );
}

void StartWorkers()
{
  int NumProcs = NumWorkers;
  thread_t *thread = malloc(NumProcs*sizeof(*thread));

  while( --NumProcs > 0 ) {
    if (thr_create(NULL, 0, Transfer, (void*)NumProcs,
      0, &thread[NumProcs])==-1)
      SisalError( "StartWorkers", "create FAILED" );
  }

  Transfer( (void*)NumProcs );
}

void StopWorkers()
{
  *SisalShutDown = TRUE;
  LeaveWorker();
}

void AbortParallel()
{
  (void)exit( 1 );
}

BARRIER_TYPE *MyInitBarrier(limit)
  int limit;
{
  BARRIER_TYPE *bar;

  if ( (bar = malloc(sizeof(BARRIER_TYPE))) == NULL ) {
    SisalError( "MyInitBarrier", "barrier malloc FAILED" );
    return NULL;
  }
  if ( MY_INIT_LOCK(&bar->lock) ) {
    SisalError( "MyInitBarrier", "barrier lock FAILED" );
    return NULL;
  }
  if ( cond_init(&bar->cond, USYNC_PROCESS, NULL) ) {
    SisalError( "MyInitBarrier", "barrier condition FAILED" );
    return NULL;
  }
  bar->count = limit;

  return bar;
}

void MyBarrier( bar )                   /* from tomig.esd.mun.ca */
  BARRIER_TYPE *bar;
{
  /*
   * Block for lock, then decrement waiting thread count.
   */
  if ( MY_LOCK(&bar->lock) ) {
    SisalError( "MyBarrier", "barrier lock FAILED" );
  }
  bar->count--;

  /*
   * Last thread broadcasts done. Others loop, checking
   * for done (relinquishing and taking the lock).
   */
  if (bar->count<1) {
    if (cond_broadcast(&bar->cond)) {
      SisalError( "MyBarrier", "barrier broadcast FAILED" );
    }
  } else {
    while (bar->count>0) {
      if (cond_wait(&bar->cond, &bar->lock)) {
        SisalError( "MyBarrier", "barrier wait FAILED" );
      }
    }
  }

  /*
   * Last thread is done (so all are done) and this thread has lock so unlock.
   */
  if (MY_UNLOCK(&bar->lock)) {
    SisalError( "MyBarrier", "barrier unlock FAILED" );
  }

  return;
}

#endif
