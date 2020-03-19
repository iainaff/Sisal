#include "world.h"


/**************************************************************************/
/* GLOBAL **************        FindLastNode       ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE LAST NODE IN THE NODE LIST BEGINNING WITH NODE n;  */
/*          NOTE n ITSELF MIGHT BE THE LAST NODE.                         */
/**************************************************************************/

PNODE FindLastNode( n )
register PNODE n;
{
  if ( n == NULL )
    return( NULL );

  while ( n->nsucc != NULL )
    n = n->nsucc;

  return( n );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:28:35  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
