#include "world.h"


/**************************************************************************/
/* GLOBAL **************      AddToEquivClass      ************************/
/**************************************************************************/
/* PURPOSE: ADD n TO THE END OF THE EQUIVALENCE CLASS HEADED BY r.        */
/**************************************************************************/

void AddToEquivClass( r, n )
PINFO r;
PINFO n;
{
  register int c;

  c = r->eid;

  n->mnext = NULL;
  n->eid   = c;

  ttable[ c ]->mnext = n;
  ttable[ c ] = n;
}

/* $Log$
 * Revision 1.1  1993/04/16  18:59:37  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:27:40  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
