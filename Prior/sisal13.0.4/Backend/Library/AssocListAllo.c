#include "world.h"


/**************************************************************************/
/* GLOBAL **************      AssocListAlloc       ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE, INITIALIZE, AND RETURN AN ASSOCIATION LIST NODE.    */
/**************************************************************************/

PALIST AssocListAlloc( datum )
int datum;
{
  register PALIST l;

  l = (PALIST) MyAlloc( (int)sizeof(ALIST) );

  l->next  = NULL;
  l->datum = datum;

  return( l );
}

/* $Log$
 * Revision 1.1  1993/04/16  18:59:55  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:27:58  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
