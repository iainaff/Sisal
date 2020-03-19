#include "world.h"


/**************************************************************************/
/* GLOBAL **************        UnlinkGraph        ************************/
/**************************************************************************/
/* PURPOSE: UNLINK GRAPH NODE g FROM ITS DOUBLE LINK LIST. IF IT EXISTS,  */
/*          THE PREDECESSOR OF g IS RETURNED, ELSE g'S SUCCESSOR IS       */
/*          RETURNED.                                                     */
/**************************************************************************/

PNODE UnlinkGraph( g )
PNODE g;
{
  register PNODE pred = g->gpred;

  if ( pred != NULL )
    pred->gsucc = g->gsucc;

  if ( g->gsucc != NULL )
    g->gsucc->gpred = pred;

  pred     = (pred != NULL)? pred : g->gsucc;
  g->gsucc = NULL;
  g->gpred = NULL;

  return( pred );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:30:37  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
