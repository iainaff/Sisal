#include "world.h"


/**************************************************************************/
/* GLOBAL **************      LinkAssocLists       ************************/
/**************************************************************************/
/* PURPOSE: LINK THE ASSOCIATION LIST lst2 TO lst1 AND RETURN THE RESULT. */
/*          IF THE FIRST LIST IS EMPTY, THE SECOND IS RETURNED.           */
/**************************************************************************/

PALIST LinkAssocLists( lst1, lst2 )
PALIST lst1;
PALIST lst2;
{
  register PALIST l;
  register PALIST prev = NULL;

  for( l = lst1; l != NULL; l = l->next )
    prev = l;

  if ( prev == NULL )
    return( lst2 );

  prev->next = lst2;

  return( lst1 );
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:27  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:29:20  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
