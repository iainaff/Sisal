#include "world.h"


/**************************************************************************/
/* GLOBAL **************     AreNodesDependent     ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF NODE n1 EXPORTS A VALUE USED BY NODE n2, ELSE  */
/*          RETURN FALSE.                                                 */
/**************************************************************************/

int AreNodesDependent( n1, n2 )
PNODE n1;
PNODE n2;
{
  register PEDGE e;

  for ( e = n1->exp; e != NULL; e = e->esucc ) {
    if ( e->dst == n2 )
      return( TRUE );

    if ( IsGraph( e->dst ) )
      continue;

    if ( AreNodesDependent( e->dst, n2 ) )
      return( TRUE );
  }

  return( FALSE );
}

/* $Log$
 * Revision 1.1  1993/04/16  18:59:40  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:27:45  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
