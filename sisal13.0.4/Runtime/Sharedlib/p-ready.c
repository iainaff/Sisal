#include "world.h"


#if !defined(NO_STATIC_SHARED)
static struct ActRecCache *ARList;
#endif


void InitReadyList()
{
  register int Index;

  ARList = (struct ActRecCache *) 
	   SharedMalloc( SIZEOF(struct ActRecCache) * NumWorkers );

  for ( Index = 0; Index < NumWorkers; Index++ ) {
    MY_INIT_LOCK( (&(ARList[Index].Mutex)) );
    ARList[Index].Head = (struct ActRec *) NULL;
    ARList[Index].Tail = (struct ActRec *) NULL;
    }
}

#define DoTheEnQ( ThisAR, LastAR, Where ) \
{ \
  register struct ActRecCache *arc; \
  arc = Where; \
  LastAR->NextAR = (struct ActRec *) NULL; \
  MY_LOCK( &(arc->Mutex) ); \
  if ( arc->Head != (struct ActRec *) NULL ) { \
    arc->Tail->NextAR = ThisAR; \
    arc->Tail = LastAR; \
  } else { \
    arc->Head = ThisAR; \
    arc->Tail = LastAR; \
    } \
  FLUSHLINE(&(LastAR->NextAR)); \
  FLUSHLINE(&(arc->Tail->NextAR)); \
  FLUSHLINE(&(arc->Head));\
  FLUSHLINE(&(arc->Tail));\
  CACHESYNC; \
  MY_UNLOCK( &(arc->Mutex) ); \
  FLUSHLINE(&(arc->Mutex)); \
}

#define DoTheEnQFast( ThisAR, LastAR, Where ) \
{ \
  register struct ActRecCache *arc; \
  arc = Where; \
  LastAR->NextAR = (struct ActRec *) NULL; \
  arc->Head = ThisAR; \
  arc->Tail = LastAR; \
  FLUSHLINE(&(LastAR->NextAR)); \
  FLUSHLINE(&(arc->Head));\
  CACHESYNC; \
}


void RListEnQ( FirstAR, LastAR )
register struct ActRec *FirstAR;
register struct ActRec *LastAR;
{
  register unsigned  int  NextPid;
  register struct ActRec *CurrAR;
  register struct ActRec *EndAR;

  FLUSH(FirstAR,LastAR+sizeof(*LastAR));

  if ( BindParallelWork ) {
    NextPid = 0;
    CurrAR  = FirstAR;
    EndAR   = LastAR + 1;

    if(OneLevelParallel)
    {
	    while (CurrAR != EndAR) {
	      DoTheEnQFast( CurrAR, CurrAR, &(ARList[NextPid]) );

	      CurrAR++;
	      NextPid++;

	      if ( NextPid == NumWorkers )
		NextPid = 0;
	      }
    }
    else
    {
	    while (CurrAR != EndAR) {
	      DoTheEnQ( CurrAR, CurrAR, &(ARList[NextPid]) );

	      CurrAR++;
	      NextPid++;

	      if ( NextPid == NumWorkers )
		NextPid = 0;
	      }
    }
    return;
    }

  DoTheEnQ( FirstAR, LastAR, &(ARList[0]) );
}


struct ActRec *RListDeQ()
{
  register struct ActRec      *ThisAR;
  register struct ActRecCache *arc;
  register int                 pID;

  if ( BindParallelWork ) {
    GETPROCID(pID);
    arc = &(ARList[pID]);
    }
  else
    arc = &(ARList[0]);

  if(!OneLevelParallel)
  {
  	MY_LOCK( &(arc->Mutex) );
  	FLUSHLINE(arc);
  }
  /*
   * we need to take this lock so that we are locked
   * when we read arc->Head.  Otherwise, when we flush back, we may
   * overwrite arc->Tail which is being written elsewhere
   */
  if ( arc->Head == (struct ActRec *) NULL )
  {
    FLUSHLINE(&(arc->Head));
    if(!OneLevelParallel)
    {
    	MY_UNLOCK( &(arc->Mutex) );
    	FLUSHLINE(&(arc->Mutex));
    }
    return( (struct ActRec *) NULL );
  }


  ThisAR = arc->Head;

  if( ThisAR != (struct ActRec *) NULL )
    arc->Head = ThisAR->NextAR;

  FLUSHLINE(&(arc->Head));

  CACHESYNC;
  if(!OneLevelParallel)
  {
  	MY_UNLOCK( &(arc->Mutex) );
  	FLUSHLINE(&(arc->Mutex));
  }

  return( ThisAR );
}
