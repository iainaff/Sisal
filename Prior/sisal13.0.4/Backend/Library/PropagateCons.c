#include "world.h"


/**************************************************************************/
/* GLOBAL **************       PropagateConst      ************************/
/**************************************************************************/
/* PURPOSE: PROPAGATE ALL CONSTANT IMPORTS OF COMPOUND NODE c INTO ITS    */
/*          SUBGRAPHS. TO CONFOM TO DI EXPECTATIONS, NEGATIVE ARITHMETIC  */
/*          CONSTANTS ARE NOT MOVED INTO SUBGRAPHS---OVERRIDDEN BY sgnok. */
/**************************************************************************/

void PropagateConst( c )
PNODE c;
{
  register PNODE g;
  register PEDGE i;
  register PEDGE si;

  for ( i = c->imp; i != NULL; i = si ) {
    si = i->isucc;

    if ( !IsConst( i ) )
      continue;

    if ( !CanPropagateConst( i ) )
      continue;

    pcnt++;

    for ( g = c->C_SUBS; g != NULL; g = g->gsucc )
      ChangeExportsToConst( g, i->iport, i );

    /* NEW CANN 12/6/90 */
    UnlinkImport( i );
  }
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:53  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:30:04  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
