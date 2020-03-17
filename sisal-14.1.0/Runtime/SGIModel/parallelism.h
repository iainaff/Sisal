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

#ifndef ALLIANT
#ifndef CRAY
#ifndef SGI
  p_procnum = 0;  /* FOR FIBRE PROCESSING??? */
#endif
#endif
#endif

#ifdef SGI
  GetProcId = 0;
#endif

/************************************************************************\
 * SGI
\************************************************************************/

static ulock_t  TheLock;
static usptr_t *UsHandle;

void ReleaseSharedMemory()
{
}

static void SgiTransfer( ProcId )
void* ProcId;
{
  GetProcId = (long)ProcId;

  if ( NumWorkers > 1 ) {
    if ( BindParallelWork )
      if ( sysmp( MP_MUSTRUN, ProcId ) == -1 )
        SisalError( "SgiTransfer", "sysmp MP_MUSTRUN FAILED" );
    if ( schedctl( SCHEDMODE, SGS_GANG, 0 ) == -1 )
      SisalError( "StartWorkers", "schedctl FAILED" );
  }

#if defined(DIST_DSA)
        if(ProcId != 0)
                InitDsa(DsaSize/(2*(NumWorkers - 1)), XftThreshold);
#endif

  EnterWorker( ProcId );

  if ( ProcId != 0 ) {
    LeaveWorker();
    _exit( 0 );
    }
}

void AcquireSharedMemory( NumBytes ) 
int NumBytes;
{
  char ArenaName[50];

  SharedSize = NumBytes + 100000;

  SPRINTF( ArenaName, "/tmp/sis%d", getpid() );

  if ( (usconfig( CONF_INITUSERS, NumWorkers )) == -1 )
    SisalError( "AcquireSharedMemory", "USCONFIG CONF_INITUSERS FAILED" );

  if ( (usconfig( CONF_ARENATYPE, US_SHAREDONLY )) == -1 )
    SisalError( "AcquireSharedMemory", "USCONFIG CONF_ARENATYPE FAILED" );

  if ( (UsHandle = usinit(ArenaName)) == NULL)
    SisalError( "AcquireSharedMemory", "USINIT FAILED" );

  if ( (TheLock = usnewlock( UsHandle )) == (ulock_t) NULL )
    SisalError( "AcquireSharedMemory", "usnewlock FAILED" );


  SharedBase = SharedMemory = (char *) malloc( SharedSize-40 );

  if ( SharedMemory == (char *) NULL )
    SisalError( "AcquireSharedMemory", "malloc FAILED" );
}


void StartWorkers()
{
  register int NumProcs = NumWorkers;

  while( --NumProcs > 0 )
    if ( sproc( SgiTransfer, PR_SADDR, NumProcs ) == -1 )
      SisalError( "StartWorkers", "sproc FAILED" );

  SgiTransfer( (void*)NumProcs );
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

void MyLock(plock)
register volatile LOCK_TYPE *plock;
{
  for (;;) {
    while (*(plock) == 1);
    ussetlock(TheLock);
    if (*(plock) == 0) {
      *(plock) = 1;
      usunsetlock(TheLock);
      break;
      }
    usunsetlock(TheLock);
    }
}

void MyUnlock(plock)
register volatile LOCK_TYPE *plock;
{
  *plock = 0;
}

void MyInitLock(plock)
register volatile LOCK_TYPE *plock;
{
  *plock = 0;
}

BARRIER_TYPE *MyInitBarrier()
{
  barrier_t *bar;

  if ( (bar = new_barrier( UsHandle )) == (barrier_t *) NULL )
    SisalError( "myinitbarrier", "new_barrier FAILED" );

  init_barrier(bar);

  return( (BARRIER_TYPE*) bar );
}

void MyBarrier( bar, limit )
BARRIER_TYPE *bar;
int limit;
{
  barrier( (barrier_t *) bar, limit );
}

#endif
