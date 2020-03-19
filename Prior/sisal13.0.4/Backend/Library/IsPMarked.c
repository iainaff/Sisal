#include "world.h"


/**************************************************************************/
/* GLOBAL **************       IsPMarked           ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF A REFERENCE TO EXPORT eport OF NODE n CARRIES  */
/*          A `P' MARK.                                                   */
/**************************************************************************/

int IsPMarked( n, eport )
PNODE n;
int   eport;
{
  register PEDGE e;

  for ( e = n->exp; e != NULL; e = e->esucc ) {
    if ( e->eport != eport )
      continue;

    if ( e->mark == 'P' || e->pmark )
      return( TRUE );
  }

  return( FALSE );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:12  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
