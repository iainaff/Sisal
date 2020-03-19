#include "world.h"


/**************************************************************************/
/* GLOBAL **************       UnlinkAdeExport     ************************/
/**************************************************************************/
/* PURPOSE: UNLINK ADE a FROM ITS SOURCE NODE'S ADE EXPORT LIST.          */
/**************************************************************************/

void UnlinkAdeExport( a )
PADE a;
{
  if ( a->epred == NULL ) {	/* FIRST ONE IN LIST */
    a->src->aexp = a->esucc;

    if ( a->esucc != NULL )
      a->esucc->epred = NULL;
  } else {
    a->epred->esucc = a->esucc;

    if ( a->esucc != NULL )
      a->esucc->epred = a->epred;
  }
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:59  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:30:34  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
