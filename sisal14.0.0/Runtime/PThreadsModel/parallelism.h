#ifndef PARALLELISM_H
#define PARALLELISM_H

/**************************************************************************/
/* FILE   **************       parallelism.h       ************************/
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

int p_procnum = 0;
static pthread_t *threads = 0;

void ReleaseSharedMemory()
{
  free( SharedBase );
}

static void* Transfer( ProcId )
void* ProcId;
{
  GetProcId = (long)ProcId;
  printf("Enter worker %d\n",GetProcID);

#if defined(DIST_DSA)
  if (ProcId != 0) {
    InitDsa(DsaSize/(2*(NumWorkers - 1)), XftThreshold);
  }
#endif

  EnterWorker( GetProcId );
  printf("Ready to leave worker %d\n",GetProcID);

  if ( GetProcId != 0 ) {
     printf("Leaving thread %d\n",GetProcID);
    LeaveWorker();
     printf("exit thread %d\n",GetProcID);
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
  threads = malloc(NumProcs*sizeof(*threads));

  while( --NumProcs > 0 ) {
     printf("Starting thread %d\n",NumProcs);
    if ( pthread_create( &thread[NumProcs], 0, Transfer,
                         (void*)NumProcs ) == -1 ) {
       SisalError( "StartWorkers", "create FAILED" );
    }
  }

     printf("Running master %d\n",NumProcs);
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
  bar->count = limit;

  return bar;
}

void MyBarrier( bar )
  BARRIER_TYPE *bar;
{
  /*
   * Block for lock, then decrement waiting thread count.
   */
  if ( MY_LOCK(&bar->lock) ) {
    SisalError( "MyBarrier", "barrier lock FAILED" );
  }
  bar->count--;
  if (MY_UNLOCK(&bar->lock)) {
    SisalError( "MyBarrier", "barrier unlock FAILED" );
  }

  /*
   * Busy-wait loop, checking for done.
   */
  while (bar->count>0) {
#ifdef HAVE_SCHED_YIELD
    sched_yield();
#else
    pthread_yield();
#endif
  }

  return;
}

#endif
