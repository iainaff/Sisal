/************************************************************************\
 * Sequential UNIX
\************************************************************************/

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
