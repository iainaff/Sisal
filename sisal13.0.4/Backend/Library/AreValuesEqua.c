#include "world.h"


/**************************************************************************/
/* GLOBAL **************     AreValuesEqual        ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF EDGE v1 AND EDGE v2 CARRY THE SAME VALUE.      */
/**************************************************************************/

int AreValuesEqual( v1, v2 )
PEDGE v1;
PEDGE v2;
{
  if ( v1 == NULL || v2 == NULL )
    return( FALSE );

  return( AreEdgesEqual( FindSource(v1), FindSource(v2) ) );
}

/* $Log$
 * Revision 1.1  1993/04/16  18:59:42  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:27:48  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
