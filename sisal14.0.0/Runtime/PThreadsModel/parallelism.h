/************************************************************************\
 * POSIX threads
\************************************************************************/

int p_procnum = 0;

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
#ifdef HAVE_SCHED_YIELD
    sched_yield();
#else
    pthread_yield();
#endif
  }

  return;
}

