#include "world.h"


/**************************************************************************/
/* GLOBAL **************      HasWriteExport       ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF NODE n HAS AN EXPORT CLASSIFIED AS A WRITE.    */
/*          SUCH AN EXPORT IS GUARANTEED TO BE AN AGGREGATE.              */
/**************************************************************************/

int HasWriteExport( n )
PNODE n;
{
  register PEDGE e;

  for ( e = n->exp; e != NULL; e = e->esucc )
    if ( e->wmark )
      return( TRUE );

  return( FALSE );
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:21  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:28:47  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
