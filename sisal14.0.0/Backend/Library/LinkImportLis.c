#include "world.h"


/**************************************************************************/
/* GLOBAL **************      LinkImportLists      ************************/
/**************************************************************************/
/* PURPOSE: LINK THE IMPORT LIST OF NODE n2 TO THAT OF NODE n1.  THE      */
/*          IMPORT LIST OF n2 IS SET TO NULL.                             */
/**************************************************************************/

void LinkImportLists( n1, n2 )
PNODE n1;
PNODE n2;
{
  register PEDGE i;
  register PEDGE si;

  for ( i = n2->imp; i != NULL; i = si ) {
    si = i->isucc;
    LinkImport( n1, i );
  }

  n2->imp = NULL;
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:32  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:29:26  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
