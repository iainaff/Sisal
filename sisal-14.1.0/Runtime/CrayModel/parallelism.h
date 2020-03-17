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
 * Cray
\************************************************************************/

int TaskInfo[MAX_PROCS][3];
LOCK_TYPE TheFirstLock;

void ReleaseSharedMemory()
{
  free( SharedBase );
}

void AcquireSharedMemory( NumBytes ) 
int NumBytes;
{
  SharedSize = NumBytes + 100000;

  SharedBase = SharedMemory = malloc( SharedSize-40 );

  if ( SharedMemory == (char *) NULL )
    SisalError( "AcquireSharedMemory", "malloc FAILED" );

  SharedMemory = ALIGN(char*,SharedMemory);
}

int ProcessorId()
{
  register int pID;
  GETPROCID(pID);
  return( pID );
}

static void CrayWorker( ProcId )
int ProcId;
{
#if defined(DIST_DSA)
        if(ProcId != 0)
                InitDsa(DsaSize/(2*(NumWorkers - 1)), XftThreshold);
#endif
  EnterWorker( ProcId );
  LeaveWorker();
}

void StartWorkers()
{
  register int NumProcs = NumWorkers;
  register int i;

  MY_LOCKASGN;

  for ( i = 0; i < NumProcs; i++ ) {
    TaskInfo[i][0] = 3;
    TaskInfo[i][2] = i;  /* PROCESS IDENTIFIER */
    }

  for ( i = 1; i < NumProcs; i++ )
    TSKSTART( TaskInfo[i], CrayWorker, TaskInfo[i][2] );

  EnterWorker( TaskInfo[0][2] );
}

void StopWorkers()
{
  register int i;

  *SisalShutDown = TRUE;
  LeaveWorker();

  for ( i = 1; i < NumWorkers; i++ )
    TSKWAIT( TaskInfo[i] );
}

void AbortParallel()
{
  ERREXIT();
}

#endif
