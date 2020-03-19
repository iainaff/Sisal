#include "world.h"


/**************************************************************************/
/* GLOBAL **************     LinkExportToEnd       ************************/
/**************************************************************************/
/* PURPOSE: ADD EDGE e TO THE TAIL OF src'S EXPORT LIST.                  */  
/**************************************************************************/


void LinkExportToEnd( src, e )
PNODE src;
PEDGE e;
{
  register PEDGE ee;

  for ( ee = src->exp; ee != NULL; ee = ee->esucc )
    if ( ee->esucc == NULL )
      break;

  e->esucc = NULL;
  e->epred = ee;
  e->src   = src;

  if ( ee == NULL ) {
    src->exp = e;
    return;
  }

  ee->esucc = e;
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:30  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:29:23  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
