#include "world.h"


/**************************************************************************/
/* GLOBAL **************        LinkGraph          ************************/
/**************************************************************************/
/* PURPOSE: LINK GRAPH NODE g TO THE DOUBLE LINK LIST CONTAINING GRAPH    */
/*          NODE pred SO TO DIRECTLY FOLLOW pred.  IF pred IS NULL, THE   */
/*          INSERTION WILL NOT TAKE PLACE; REGUARDLESS, NODE g IS ALWAYS  */
/*          RETURNED.                                                     */
/**************************************************************************/

PNODE LinkGraph( pred, g )
PNODE pred;
PNODE g;
{
  if ( pred == NULL )
    return( g );

  g->gsucc = pred->gsucc;
  g->gpred = pred;

  if ( pred->gsucc != NULL )
    pred->gsucc->gpred = g;

  pred->gsucc = g;

  return( g );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:24  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
