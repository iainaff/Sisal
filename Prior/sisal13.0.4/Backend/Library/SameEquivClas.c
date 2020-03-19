#include "world.h"


/**************************************************************************/
/* GLOBAL **************      SameEquivClass       ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF a and b ADDRESS TYPES IN THE SAME EQUIVALENCE  */
/*          CLASS.                                                        */
/**************************************************************************/

#define EquivClass(x) (((x) == NULL)? -1 : (x)->eid)

int SameEquivClass( a, b )
PINFO a;
PINFO b;
{
  return( (EquivClass( a->info1 ) == EquivClass( b->info1 )) &&
	 (EquivClass( a->info2 ) == EquivClass( b->info2 )) );
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:56  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:30:22  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
