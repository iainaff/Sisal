/************************************************************************\
 * p-ppp.c - SISAL runtime system machine-specific parallel processing
 *           primitives
\************************************************************************/

#include "world.h"

#if !defined(NO_STATIC_SHARED)
int sdebug = 1;
static char *SharedBase; 
static char *SharedMemory; 
static int   SharedSize;
#else
struct shared_s *SRp;
#endif

void StartWorkers();

#if !defined(NO_STATIC_SHARED)
void (*Entry_point)();
#endif

void StartWorkersWithEntry(entry)
void (*entry)();
{
	Entry_point = entry;
	StartWorkers();
#if !defined(POWER4)
	Entry_point();
#endif
}


POINTER SharedMalloc( NumBytes )
int NumBytes;
{
  register char *ReturnPtr;

#if defined(NON_COHERENT_CACHE)
  NumBytes += CACHE_LINE;
#else
  NumBytes += 50;
#endif
  NumBytes = ALIGN( int, NumBytes );

  if ( SharedSize < NumBytes )
    SisalError( "SharedMalloc", "ALLOCATION SIZE TO BIG" );

  ReturnPtr     = SharedMemory;
  SharedMemory += NumBytes;
  SharedSize   -= NumBytes;

  return( (POINTER) ReturnPtr );
}

/************************************************************************\
 * Encore
\************************************************************************/

#ifdef ENCORE
#define PPPDEFINED

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

/************************************************************************\
 * Alliant
\************************************************************************/

#ifdef ALLIANT
#define PPPDEFINED

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

void StartWorkers()
{
  EnterWorker( 0 );
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

/************************************************************************\
 * Sequent
\************************************************************************/

#if defined(BALANCE) || defined(SYMMETRY)
#define PPPDEFINED

extern char *shmalloc();

void AcquireSharedMemory( NumBytes ) 
int NumBytes;
{
  SharedSize = NumBytes + 100000;

  SharedBase = SharedMemory = shmalloc( SharedSize-40 );

  if ( SharedMemory == (char *) NULL )
    SisalError( "AcquireSharedMemory", "shmalloc FAILED" );

  SharedMemory = ALIGN(char*,SharedMemory);
}

void ReleaseSharedMemory()
{
  shfree( SharedBase );
}

#ifdef GANGD
void StartWorkers()
{
  register int pID;

  begin_parallel( NumWorkers );

  GETPROCID(pID);

#if defined(DIST_DSA)
        if(pID != 0)
                InitDsa(DsaSize/(2*(NumWorkers - 1)), XftThreshold);
#endif

  EnterWorker( pID );

  if ( pID != 0 ) {
    LeaveWorker();
    end_parallel();
    }
}

void StopWorkers()
{
  *SisalShutDown = TRUE;
  LeaveWorker();
  end_parallel();
}

void AbortParallel() 
{ 
  abort_parallel(); 
}
#else
int p_procnum = 0;

void StartWorkers( )
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

#endif

/************************************************************************\
 * Cray
\************************************************************************/

#ifdef CRAY
#define PPPDEFINED

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

/************************************************************************\
 * SGI
\************************************************************************/

#ifdef SGI
#define PPPDEFINED

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

/************************************************************************\
 * IBM Power4
\************************************************************************/

#if defined(POWER4)
#define PPPDEFINED

/*
 * This is just the way that IBM does it and they, obviously, know
 * everything.
 *
 * WARNING: The system's shared variables are located in the first
 * 1K of the shard memory segment.  If it is ever the case that there
 * are more than 1K of shared variables, get out of Dodge fast.
 */
#define SHMEMBASE 0x30001000


void ReleaseSharedMemory()
{
	return;
}

void AcquireSharedMemory( NumBytes ) 
int NumBytes;
{
  SharedSize = NumBytes + 100000;

  /*
   * attach the shared region
   */
   p6k_mshmat(SHMEMBASE, SharedSize);
  /*
   * that is the address at which it begins -- trust me
   */
  SharedBase = SharedMemory = SHMEMBASE;

  if ( SharedMemory == (char *) NULL )
    SisalError( "AcquireSharedMemory", "malloc FAILED" );

  SharedMemory = ALIGN(char*,SharedMemory);
}

void StopWorkers()
{
  *SisalShutDown = TRUE;
  FLUSHLINE(SisalShutDown);
  LeaveWorker();
}

int p_procnum;
int *pid;
LOCK_TYPE pidlock;

extern BARRIER_TYPE *StartBarrier;
extern BARRIER_TYPE *FinishBarrier;

void GoneParallel()
{
	p6k_whoami(&p_procnum);

	StartBarrier = &(SR.Sb);
	FinishBarrier = &(SR.Fb);

	/*
	 * so the workers will see the shared region
	 */
	SRp = &SR;

#if defined(DIST_DSA)
	if(p_procnum != 0)
		InitDsa(DsaSize/(2*(NumWorkers - 1)), XftThreshold);
#endif

	EnterWorker(p_procnum);

	if(p_procnum == 0)
	{
		if(Entry_point == NULL)
			SisalMain( SisalMainArgs );
		else
			Entry_point();
		StopWorkers();
	}
	else
	{
		LeaveWorker();
	}
}

void StartWorkers() 
{
	/*
	 * Flush cache to make sure that all shared variables are
	 * visible
	 */
	FLUSHALL;
	/*
	 * This is SPMD.  When the other processes terminate, the call
	 * returns.  We, therefore, can't start SisalMain in p-srt0.c
	 * because there is no way to start all of the workers but 1, and
	 * no way to return back to it once we have gone parallel.
	 */
        p6k_pcall(GoneParallel, 0xFFFF0003, 0);

}


void AbortParallel() 
{ 
  exit( 1 ); 
}

#endif

/************************************************************************\
 * Solaris threads
\************************************************************************/

#ifdef STHREADS
#define PPPDEFINED

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

void MyBarrier( bar )			/* from tomig.esd.mun.ca */
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

/************************************************************************\
 * POSIX threads
\************************************************************************/

#if defined(PTHREADS)
#define PPPDEFINED

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
    pthread_exit( 0 );
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
  pthread_t *thread = malloc(NumProcs*sizeof(*thread));

  while( --NumProcs > 0 ) {
    if ( pthread_create( &thread[NumProcs], (void*)NumProcs, Transfer,
      (void*)NumProcs ) == -1 )
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
    pthread_yield();
  }

  return;
}

#endif /* PTHREADS */

/************************************************************************\
 * Sequential UNIX
\************************************************************************/

#ifndef PPPDEFINED
#define PPPDEFINED

int p_procnum = 0;

void ReleaseSharedMemory()
{
  free( SharedBase );
}

void AcquireSharedMemory( NumBytes ) 
int NumBytes;
{
  SharedSize = NumBytes + 100000;

  SharedBase = SharedMemory = (char *)malloc( SharedSize-40 );

  if ( SharedMemory == (char *) NULL )
    SisalError( "AcquireSharedMemory", "malloc FAILED" );
}

void StartWorkers() 
{
#if defined(DIST_DSA)
        if(p_procnum != 0)
                InitDsa(DsaSize/(2*(NumWorkers - 1)), XftThreshold);
#endif

  EnterWorker( p_procnum );
}

void StopWorkers()
{
  *SisalShutDown = TRUE;
  LeaveWorker();
}

void AbortParallel() 
{ 
  exit( 1 ); 
}
#endif

