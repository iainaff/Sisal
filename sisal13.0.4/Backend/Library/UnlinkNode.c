#include "world.h"


/**************************************************************************/
/* GLOBAL **************         UnlinkNode        ************************/
/**************************************************************************/
/* PURPOSE: UNLINK NODE n FROM ITS DOUBLE LINK LIST. IF IT EXISTS, THE    */
/*          PREDECESSOR OF n IS RETURNED, ELSE n'S SUCCESSOR IS RETURNED. */
/**************************************************************************/

PNODE UnlinkNode( n )
PNODE n;
{
  register PNODE pred = n->npred;

  nchange = TRUE;

  if ( pred != NULL )
    pred->nsucc = n->nsucc;

  if ( n->nsucc != NULL )
    n->nsucc->npred = pred;

  pred     = (pred != NULL)? pred : n->nsucc;
  n->nsucc = NULL;
  n->npred = NULL;

  return( pred );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:30:40  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
