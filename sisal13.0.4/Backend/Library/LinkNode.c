#include "world.h"


/**************************************************************************/
/* GLOBAL **************         LinkNode          ************************/
/**************************************************************************/
/* PURPOSE: LINK NODE n TO THE DOUBLE LINK LIST CONTAINING NODE pred SO   */
/*          TO FOLLOW pred. IF pred IS NULL, THE INSERTION IS NOT DONE;   */
/*          REGUARDLESS, NODE n IS RETURNED.                              */
/**************************************************************************/

PNODE LinkNode( pred, n )
PNODE pred;
PNODE n;
{
  nchange = TRUE;

  if ( pred == NULL )
    return( n );

  n->nsucc = pred->nsucc;
  n->npred = pred;

  if ( pred->nsucc != NULL )
    pred->nsucc->npred = n;

  pred->nsucc = n;

  return( n );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:27  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
