#include "world.h"


/**************************************************************************/
/* GLOBAL **************     ClearTouchFlags       ************************/
/**************************************************************************/
/* PURPOSE: CLEAR ALL THE SYMBOL TABLE TOUCH FLAGS.                       */
/**************************************************************************/

void ClearTouchFlags()
{
  register PINFO i;

  for ( i = ihead; i != NULL; i = i->next ) {
    i->touch1 = FALSE;
    i->touch2 = FALSE;
    i->touch3 = FALSE;
  }
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:04  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:28:09  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
