/************************************************************************\
 * Sequent
\************************************************************************/

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

