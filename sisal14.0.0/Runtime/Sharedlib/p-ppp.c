/************************************************************************\
 * p-ppp.c - SISAL runtime system machine-specific parallel processing
 *           primitives
\************************************************************************/

#include "sisalrt.h"

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
	Entry_point();
}


POINTER SharedMalloc( NumBytes )
int NumBytes;
{
  register POINTER ReturnPtr;

  NumBytes = ALIGN( int, NumBytes );

  if ( SharedSize < NumBytes )
    SisalError( "SharedMalloc", "ALLOCATION SIZE TO BIG" );

  ReturnPtr     = SharedMemory;
  SharedMemory += NumBytes;
  SharedSize   -= NumBytes;

  return( (POINTER) ReturnPtr );
}

#include "parallelism.h"
