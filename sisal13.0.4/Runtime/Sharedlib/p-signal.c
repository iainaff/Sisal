#include "world.h"


#ifdef CRAY

static void HandleSig( SigCode )
int SigCode;
{
  extern void InitSignalSystem();

  if ( UsingSdbx )
    switch ( SigCode ) {
      case SIGINT:
	SdbxMonitor( SDBX_INT );
	InitSignalSystem();
	return;

      case SIGFPE:
	SdbxMonitor( SDBX_FPE );
	break;

      default:
	break;
      }

  FPRINTF( stderr, "\nERROR: SignalCode=%d\n", SigCode );
  AbortParallel();
}

void InitSignalSystem()
{
  signal( SIGINT, HandleSig );
  signal( SIGFPE, HandleSig );
}

#else


static void HandleSig( SigCode )
int SigCode;
{
  extern void InitSignalSystem();

  if ( UsingSdbx )
    switch( SigCode ) {
      case SIGINT:
	SdbxMonitor( SDBX_INT );
	InitSignalSystem();
	return;

      case SIGFPE:
	SdbxMonitor( SDBX_FPE );
	break;

      default:
	break;
      }

#if defined(POWER4)
  if(UnderDBX && (SigCode == SIGINT))
	return;
#endif

  psignal( SigCode, "\nERROR"  );
  AbortParallel();
}

void InitSignalSystem()
{
     /* DYNIX V2.1: shbrk and shsbrk require default SIGSEGV handler for    */
     /*             internal use (extension of shared memory)               */
     /*             Since shmalloc only called once from virtual processor  */
     /*             0, handler for segmentation faults can be used if setup */
     /*             after the shmalloc call (which is the case)             */
     /*             This is not done by libppp.a                            */

  (void)signal( SIGHUP,   HandleSig );
  (void)signal( SIGINT,   HandleSig );
  (void)signal( SIGQUIT,  HandleSig );
  (void)signal( SIGILL,   HandleSig );
  (void)signal( SIGFPE,   HandleSig );
  (void)signal( SIGBUS,   HandleSig );
  (void)signal( SIGPIPE,  HandleSig );
#if !defined(HPUX) && !defined(CRAYT3D)
  (void)signal( SIGXCPU,  HandleSig );
  (void)signal( SIGXFSZ,  HandleSig );
/*
  (void)signal( SIGSEGV,  HandleSig );
*/
#endif
}
#endif
