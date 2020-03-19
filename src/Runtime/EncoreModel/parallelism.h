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
 * Encore
\************************************************************************/

int p_procnum = 0;
char *share_malloc();

void ReleaseSharedMemory()
{
}

void AcquireSharedMemory( NumBytes ) 
int NumBytes;
{
  SharedSize = NumBytes + 100000;

  if ( share_malloc_init( SharedSize+100000 ) != 0 )
    SisalError( "AcquireSharedMemory", "share_malloc_init FAILED" );

  SharedBase = SharedMemory = share_malloc( SharedSize-40 );

  if ( SharedMemory == (char *) NULL )
    SisalError( "AcquireSharedMemory", "share_malloc FAILED" );

  SharedMemory = ALIGN(char*,SharedMemory);
}

void StartWorkers()
{
  register int NumProcs = NumWorkers;

  while( --NumProcs > 0 )
    if ( fork() == 0 )
      break;

#if defined(DIST_DSA)
        if(NumProcs != 0)
                InitDsa(DsaSize/(2*(NumWorkers - 1)), XftThreshold);
#endif

  EnterWorker( p_procnum = NumProcs );

  if ( NumProcs != 0 ) {
    LeaveWorker();
    exit( 0 );
    }
}


void StopWorkers()
{
  *SisalShutDown = TRUE;
  LeaveWorker();
}

void AbortParallel()
{
  (void)kill( 0, SIGKILL );
}

#endif
