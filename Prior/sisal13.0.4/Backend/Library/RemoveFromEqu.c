#include "world.h"


/**************************************************************************/
/* GLOBAL **************   RemoveFromEquivClass    ************************/
/**************************************************************************/
/* PURPOSE: REMOVE ENTRY x FROM THE EQUIVALENCE CLASS CONTAINING o.       */
/**************************************************************************/

void RemoveFromEquivClass( o, x )
PINFO o;
PINFO x;
{
  if ( x == ttable[ o->eid ] )
    ttable[ o->eid ] = o;

  o->mnext = x->mnext;
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:55  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:30:19  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
