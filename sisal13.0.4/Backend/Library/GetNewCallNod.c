#include "world.h"


/**************************************************************************/
/* GLOBAL **************      GetNewCallNode       ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE AND RETURN A NEW CALL STRUCTURE.                     */
/**************************************************************************/

PCALL GetNewCallNode()
{
  PBBLOCK b;

  b = MyBBlockAlloc();
  return( &(b->c) );
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:18  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:28:44  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
