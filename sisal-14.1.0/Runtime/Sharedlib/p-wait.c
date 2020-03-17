/**************************************************************************/
/* FILE   **************          p-wait.c         ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

#include "sisalrt.h"


static void DoDependentWork()
{
  register struct ActRec *NewAR;
  register int local_flush;

  if ( (NewAR = RListDeQ()) != (struct ActRec *) NULL ) {
    (*(NewAR->ChildCode))( NewAR->ArgPointer, 
                           NewAR->SliceBounds[1],
                           NewAR->SliceBounds[2],
                           NewAR->SliceBounds[0]
                          );

    NewAR->Done = TRUE;
    /*
     * do it this way so that if we don't flush, we don't
     * have an invalid copy of NewAR in the cache
     */
    local_flush = NewAR->Flush;
    /*
     * must flush the done flag so that the master can see it
     */
    FLUSHLINE(&(NewAR->Done));
    /*
     * if we need to invalidate the caches
     */
    if(local_flush == TRUE)
    {
        CACHESYNC;
        FLUSHALL;
    }
  }

}

#ifndef YIELD_IF_REQUIRED
#define YIELD_IF_REQUIRED()
#endif

void Wait( Event ) 
int Event;
{
  switch ( Event ) {
    case FOR_NOTHING:
        DoDependentWork();
        YIELD_IF_REQUIRED();
      break;

    case FOR_SHUTDOWN:
      while ( !(*SisalShutDown) )
      {
          DoDependentWork();
          FLUSHLINE(SisalShutDown);
          YIELD_IF_REQUIRED();
      }

      break;

    default:
      SisalError( "Wait", "ILLEGAL EVENT ENCOUNTERED" );
    }
}

void Sync( First, LastPlusOne )
register struct ActRec *First;
register struct ActRec *LastPlusOne;
{
  if ( First == LastPlusOne )
    return;

  for ( ; ; ) {
    while ( First->Done ) {
      First->Done = FALSE;
      FLUSHLINE(&(First->Done));
      First++;

      if ( First == LastPlusOne )
        return;
      }

    FLUSHLINE(&(First->Done));

    Wait( FOR_NOTHING );
    }
}
