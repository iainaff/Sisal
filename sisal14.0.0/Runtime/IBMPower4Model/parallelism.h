/************************************************************************\
 * IBM Power4
\************************************************************************/

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


