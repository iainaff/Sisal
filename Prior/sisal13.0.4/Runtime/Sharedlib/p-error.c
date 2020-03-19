#include "world.h"


static LOCK_TYPE *ErrorLock = (LOCK_TYPE*) NULL;


void InitErrorSystem()
{
  ErrorLock = (LOCK_TYPE*) SharedMalloc( SIZEOF(LOCK_TYPE) );
  MY_SINIT_LOCK(ErrorLock);
}


int SisalError( Message1, Message2 )
char *Message1;
char *Message2;
{
  if ( ErrorLock != (LOCK_TYPE*) NULL )
    MY_SLOCK( ErrorLock );

  FPRINTF( stderr, "\nERROR: (%s) %s\n", Message1, Message2 );

  if ( UsingSdbx )
    SdbxMonitor( SDBX_ERR );

  AbortParallel();

  return 0;
}
