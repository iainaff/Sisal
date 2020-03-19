#include "world.h"


/**************************************************************************/
/* GLOBAL **************       UnlinkImport        ************************/
/**************************************************************************/
/* PURPOSE: UNLINK IMPORT i FROM ITS DESTINATION NODE's IMPORT LIST.      */
/**************************************************************************/

void UnlinkImport( i )
PEDGE i;
{
  echange = TRUE;

  if ( i->ipred == NULL ) {	/* FIRST ONE IN LIST */
    i->dst->imp = i->isucc;

    if ( i->isucc != NULL )
      i->isucc->ipred = NULL;
  } else {
    i->ipred->isucc = i->isucc;

    if ( i->isucc != NULL )
      i->isucc->ipred = i->ipred;
  }
}

/* $Log$
 * Revision 1.3  1994/07/21  16:33:12  denton
 * Removed NULL assignment just put in to detect bad memory access.  Will fix later
 *
 * Revision 1.1  1993/01/21  23:30:39  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
