#include "world.h"


/**************************************************************************/
/* GLOBAL **************      UnlinkAdeImport      ************************/
/**************************************************************************/
/* PURPOSE: UNLINK ADE IMPORT a FROM ITS DESTINATION NODE's ADE IMPORT    */
/*          LIST.                                                         */
/**************************************************************************/

void UnlinkAdeImport( a )
PADE a;
{
  if ( a->ipred == NULL ) {	/* FIRST ONE IN LIST */
    a->dst->aimp = a->isucc;

    if ( a->isucc != NULL )
      a->isucc->ipred = NULL;
  } else {
    a->ipred->isucc = a->isucc;

    if ( a->isucc != NULL )
      a->isucc->ipred = a->ipred;
  }
}

/* $Log$
 * Revision 1.1  1993/04/16  19:01:00  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:30:35  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
