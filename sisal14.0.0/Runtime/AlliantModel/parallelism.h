/************************************************************************\
 * Alliant
\************************************************************************/

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

